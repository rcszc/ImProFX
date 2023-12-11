# ImProFx - CoreFX
- [x] Update.20231208 RCSZ

[返回使用文档](improfx_usage.md)
- 所在目录: src/imgui_profx_src/improfx_core/

> 2023_11_02 version 1.0.0 Alpha
> 2023_11_05 version 1.1.0 Alpha
> 2023_11_21 version 2.0.0 Test-1
> 2023_12_02 version 2.0.0 Test-2
> 2023_12_08 version 2.0.0 Alpha

CoreFX为框架核心部分.

```cpp
class CoreModuleImGui::RenderImGui;   // 框架ImGui初始化
class CoreModuleWindow::RenderWindow; // 框架GLFW窗口
class FrameworkImCore;                // 框架核心
class ProfxSystemFramework;           // 框架启动
class ProfxReflection;                // 框架反射(200T)
```

OpenGL4.6图形底层(着色器)调用封装定义: src\imgui_profx_src\improfx_shader\

## 2.0.0A
> 配置文件, ShaderFx的调用, 工具的调用, 反射使用

### 配置文件结构
配置文件使用".json"文件, 使用RapidJSON库解析, 通常此框架需要两个必要核心配置文件"window_config.json"和"shader_config.json", 前者为配置GLFW窗口和ImGui的初始化等, 后者为配置加载OpenGL着色器文件&资源; 加载顺序为 window_config => shader_config.

"window_config.json"具体参见注释:
```cpp
{
  "window_name": "ImProFX - V2.0 ALPHA",                // GLFW窗口标题
  "window_size": [ 1680, 945 ],                         // GLFW窗口大小[ width,height ]
  "window_MSAA": 4,                                     // GLFW抗锯齿(MSAA)等级
  "window_fw_version": [ 3, 3 ],                        // GLFW版本配置 x.x
  "window_sh_version": "#version 330 core",             // ImGui配置着色器版本
  "window_icon_file": "imgui_profx_system/xxx.xxx",     // 窗口图标文件
  "imgui_fonts_file": "imgui_profx_system/xxx.xxx",     // ImGui加载字体(全局)
  "imgui_fonts_size": 32.0,                             // ImGui配置字体大小(全局)
  "imgui_fonts_color": [ 0.0, 1.0, 0.85, 1.0 ],         // ImGui配置字体颜色(全局)
  "imgui_window_rounding": 5.2,                         // ImGui窗口圆角程度
  "imgui_frame_rounding": 5.5,                          // ImGui帧控件圆角程度
  "shader_reg_file": "imgui_profx_system/config/shader_config.json", // 着色器配置文件路径
  "system_create_workers": 32                           // 框架线程池初始工作线程数量
}
```

"shader_config.json"具体参见注释(文件路径对应以上"shader_reg_file"配置项):
```cpp
{
  "core_shaderfx_config": [ // 着色器包(package)数组
    {
      "shaderfx_package": "aaaxxx",                               // 包名称Key(Unique)
      "shader_file_vertex": "imgui_profx_system/shader/xxx.vs",   // 顶点着色器代码文件路径
      "shader_file_fragment": "imgui_profx_system/shader/xxx.fs", // 片元着色器代码文件路径
      "shader_fbo_texture_name": "xxx",                           // fbo附着的纹理名称(Unique)
      "render_max_fps": 200,                                      // 当前着色器渲染最大帧率(微秒级计时器)
      "render_scale_xy": [ 1.0,1.0 ],                             // 当前着色器渲染缩放(如果为[ -1,-1 ]那么默认为正常缩放)
      "shader_uniform_texture": [ // 着色器采样纹理(通过Uniform传入着色器的纹理, 占用全局TMU资源)
        {
          "image_file_path": "xxx/xxx.png", // 纹理图片文件路径
          "image_ufh_name": "xxx"           // 着色器内"Uniform"纹理名称(1i[sampler2D])
        }
      ],
      "shader_uniform_param": [ "aaa", "bbb" ] // 着色器额外"Uniform"参数名称(暂时全部类型为float)
    }
  ],
  "core_raw_texture_config": [ // 源图片纹理(不通过纹理采样器传入着色器[不占用TMU资源], ImGui直接通过句柄调用)
    {
      "image_file_path": "xxx/xxx.png", // 纹理图片文件路径
      "image_unique_name": "xxx"        // 纹理名称(调用时的Key)
    }
  ]
}
```

"core_shaderfx_config"框架内定义.
```cpp
// framework_core.hpp
std::unordered_map<std::string, FxRenderItem> CoreModuleIMFX::FRMCORE_PACKAGE::DataFxRender;
```

"core_raw_texture_config"框架内定义.
> 如果是直接ImGui调用的图片就使用此
```cpp
// framework_core.hpp
std::unordered_map<std::string, FStextureHandle> CoreModuleIMFX::FRMCORE_PACKAGE::DataRawTexture;
```

简略流程: 加载着色器项(包)(每个包内一个独立着色器程序) -> 每个包通过FBO附着渲染到附着纹理"shader_fbo_texture_name"(而非屏幕) -> 查询搜索对应包 -> 通过ImGui调用纹理句柄渲染到GUI界面(屏幕).

着色器中框架提供三个默认"Uniform":
```glsl
uniform mat4  FxMatMvp;  // Ortho2D MVP矩阵
uniform vec2  FxWinSize; // 窗口大小
uniform float FxTime;    // 每个包的独立计时器(虚拟Tick)
```

顶点着色器到片元着色器IO:
```glsl
// Vert => Frag
out vec4 FxColor; // 混合颜色(没用)
out vec2 FxCoord; // 绘制纹理坐标
// Frag => Out
out vec4 FragColor; // 输出片元
```
---

## 调用&操作核心工具
> namespace ImProFxTools

通过全局对象调用操作工具(以下为定义):
```cpp
ImProFxTools::FrameworkDataFind   ImProFxTools::FrmData;   // 数据处理(查找修改处理着色器资源&源贴图资源)
ImProFxTools::FrameworkStaticFind ImProFxTools::FrmStatic; // 静态资源(申请查找删除静态资源[这指在Loop中保持状态的资源])
ImProFxTools::FrameworkTaskFind   ImProFxTools::FrmTask;   // 异步任务(创建获取异步&多线程任务[非多线程]和结果)
```

### ImProFxTools::FrmData
> 直接通过全局对象调用以下工具 ImProFxTools::FrmData.Find...

着色器包结构体定义"FxRenderItem":
```cpp
struct FxRenderItem {

	FSmodelHanlde ModelHandle; // OpenGL模型句柄(VAO,VBO)

	FSGLuint ShaderProgramHandle; // OpenGL着色器程序句柄
	FSGLuint FrameBufferHandle;   // OpenGL帧缓冲句柄(FBO)

	FStextureHandle FrameBufferTex; // FBO附着纹理的句柄

	float     RenderTimer;     // 着色器渲染计时器(计数器)
	FSmatrix4 RenderMatrixMvp; // 着色器渲染MVP矩阵

	std::chrono::steady_clock::time_point RenderRateTimer; // 着色器帧率限制计时器
	int64_t                               RenderRateTime;  // 着色器帧率限制时间

	std::vector<FStextureHandle>           ParamTexture;    // 着色器资源纹理句柄数组
	std::unordered_map<std::string, float> ParamValueFloat; // 着色器额外参数数组(float)

	bool ExecuteProgramFlag; // 着色器程序执行标志(false:暂行运行)
};
```

查询获取渲染项(着色器包), "FxRenderItem"结构体参见以上.
```cpp
// @param  "input" (FrmDat.DataShaderFx), "itemkey" (Key包名称"shaderfx_package"配置项)
// @return FxRenderItem
FxRenderItem FindRenderItem(std::unordered_map<std::string, FxRenderItem>& input, const char* itemkey);
```

查询获取渲染项(着色器包), FBO附着纹理(着色器渲染完成的图像).
```cpp
// @param  "input" (FrmDat.DataShaderFx), "itemkey" (Key包名称"shaderfx_package"配置项)
// @return OpenGL纹理句柄
uint32_t FindRenderItemFBO(std::unordered_map<std::string, FxRenderItem>& input, const char* itemkey);
```

查询获取设置渲染项(着色器包), 自定义(着色器额外参数)"Uniform"变量float.
```cpp
// @param "input" (FrmDat.DataShaderFx), "itemkey" (Key包名称"shaderfx_package"配置项)
// @param "ufn" (变量名称"shader_uniform_param"配置项), "value" (设置值)
void FindSetRenderItemParam(
    std::unordered_map<std::string, FxRenderItem>& input, 
    const char* itemkey, 
    const char* ufn, 
    float       value
);
```

查询获取设置渲染项(着色器包), 渲染状态(着色器运行标志).
```cpp
// @param "input" (FrmDat.DataShaderFx), "itemkey" (Key包名称"shaderfx_package"配置项)
// @param "flag" (设置状态 true:运行, false:暂停)
void FindSetRenderItemFlag(std::unordered_map<std::string, FxRenderItem>& input, const char* itemkey, bool flag);
```

查询获取Raw纹理项(与着色器无关不占用TMU资源"core_raw_texture_config"配置项).
```cpp
// @param  "input" (FrmDat.DataTextureRaw), "itemkey" (Key纹理名称"image_unique_name"配置项)
// @return OpenGL纹理句柄
uint32_t FindRawTextureImg(std::unordered_map<std::string, FStextureHandle>& input, const char* itemkey);
```

查询动态加载Raw纹理项(颜色数据):
```cpp
struct SrcImage {
	uint8_t* imgpixel_ptr;                        // 图片像素源数据指针
	uint32_t img_width, img_height, img_channels; // 图片宽度.高度.颜色通道数量(RGB, RGBA)
};

// @param  "input" (FrmDat.DataTextureRaw), "itemkey" (Key纹理名称), "data" 图片源数据
// @return true:加载成功, false:加载失败
bool FindLoadRawTextureImgData(std::unordered_map<std::string, FStextureHandle>& input, strkey itemkey, const SrcImage& data);
```

查询动态加载Raw纹理项(图片文件):
```cpp
// @param  "input" (FrmDat.DataTextureRaw), "itemkey" (Key纹理名称), "file" 图片文件路径(使用stb_image加载)
// @return true:加载成功, false:加载失败
bool FindLoadRawTextureImgFile(std::unordered_map<std::string, FStextureHandle>& input, strkey itemkey, const std::string& file);
```

__简单Demo(框架内注册用户类中获取着色器FBO和源纹理句柄):__
```cpp
FrmData.FindRenderItemFBO(FrmDat.DataShaderFx, "MyShader");
FrmData.FindRawTextureImg(FrmDat.DataTextureRaw, "MyTexture");
```
---

### ImProFxTools::FrmStatic
> 直接通过全局对象调用以下工具 ImProFxTools::FrmStatic.Static...

申请静态标志(bool), 可在Init和Loop部分.
```cpp
// @param  "input" (FrmDat.StaticFlag), "itemkey" (Key标识), "init" (初始化标志)
// @param  "duplicate" (查重 true:如果遇到相同"text"[Key]则不覆盖写入)
// @return true:申请成功, false:申请失败
bool StaticFlagAlloc(std::unordered_map<std::string, bool>& input, const char* itemkey, bool init = false, bool duplicate = false);
```

删除静态标志(bool) delete item(key-value).
```cpp
// @param  "input" (FrmDat.StaticFlag), "itemkey" (Key标识)
// @param  "erase" (释放对象后是否擦除Hashmap键值对[此处必须为true因为非对象指针])
// @return true:删除成功, false:删除失败
bool StaticFlagDelete(std::unordered_map<std::string, bool>& input, const char* itemkey, const bool& erase = true);
```

查询获取静态标志(bool)指针.
```cpp
// @param "input" (FrmDat.StaticFlag), "itemkey" (Key标识)
// @return bool_pointer
bool* StaticFlagFind(std::unordered_map<std::string, bool>& input, const char* itemkey);
```

静态计时器对象函数(计时精度:微秒, 单位: 毫秒):
```cpp
// 循环触发计时器: "itl_time" 触发间隔时间, "loss_time" 获取触发时间误差(比如: 触发时间为500ms当前循环时刻为505ms, loss = 5ms) 
bool ChronoTimer::PeriodicTimerFlag(float itl_time, float* loss_time = nullptr);
// 上下文时间测定计时器: "context_function" 上下文函数对象(测算此函数调用运行时间)
float ChronoTimer::ContextTimerUse(std::function<void()> context_function);
```

申请静态计时器对象, 可在Init和Loop部分.
```cpp
// @param  "input" (FrmDat.StaticTimer), "itemkey" (Key标识) "windowsize" (当前窗口大小)
// @return true:申请成功, false:申请失败
bool StaticTimerAlloc(std::unordered_map<std::string, ChronoTimer*>& input, strkey itemkey);
```

删除(释放)静态计时器对象 delete object & item(key-value).
```cpp
// @param  "input" (FrmDat.StaticTimer), "itemkey" (Key标识)
// @param  "erase" (释放对象后是否擦除Hashmap键值对)
// @return true:删除成功, false:删除失败
bool StaticTimerDelete(std::unordered_map<std::string, ChronoTimer*>& input, strkey itemkey, const bool& erase = true);
```

查询获取静态计时器对象指针.
```cpp
// @param  "input" (FrmDat.StaticTimer), "itemkey" (Key标识)
// @return object_pointer
ChronoTimer* StaticTimerFind(std::unordered_map<std::string, ChronoTimer*>& input, strkey itemkey);
```

申请静态粒子系统, 可在Init和Loop部分.
```cpp
// @param "input" (FrmDat.ParticleSystem), "itemkey" (Key标识)
// @return true:申请成功, false:申请失败
bool StaticParticleSysAlloc(std::unordered_map<std::string, IFC_PARTC::ParticleBase*>& input, strkey itemkey, const Vector2T<float>& windowsize);
```

删除(释放)静态粒子系统 delete object & item(key-value).
```cpp
// @param  "input" (FrmDat.ParticleSystem), "itemkey" (Key标识)
// @param  "erase" (释放对象后是否擦除Hashmap键值对)
// @return true:删除成功, false:删除失败
bool StaticParticleSysDelete(std::unordered_map<std::string, IFC_PARTC::ParticleBase*>& input, strkey itemkey, const bool& erase = true);
```

查询获取静态粒子系统对象指针, 具体操作请看: [框架粒子系统文档](improfx_particle.md)
```cpp
// @param  "input" (FrmDat.ParticleSystem), "itemkey" (Key标识)
// @return object_pointer
IFC_PARTC::ParticleBase* StaticParticleSysFind(std::unordered_map<std::string, IFC_PARTC::ParticleBase*>& input, strkey itemkey);
```

__简单Demo(框架内注册用户类中调用静态标志和静态计时器):__
```cpp
auto MyFlag = FrmStatic.StaticFlagFind(FrmDat.StaticFlag, "MyStaticFlag");
FrmStatic.StaticTimerFind(FrmDat.StaticTimer, "MyStaticTimer")->ContextTimerUse(...);
```
---

### ImProFxTools::FrmTask
> 直接通过全局对象调用以下工具 ImProFxTools::FrmTask.TaskAsync...
- 当前还在开发阶段, 异步任务无返回值(void) - 20231202

创建异步任务(注意是"shared_future"而非"future")
```cpp
// @param "input" (FrmDat.TaskAsync), "itemkey" (Key标识)
void TaskAsyncCreate(std::unordered_map<std::string, std::shared_future<void>>& input, strkey itemkey, std::future<void> async);
```

等待异步任务完成(暂时无返回值)
```cpp
// @param  "input" (FrmDat.TaskAsync), "itemkey" (Key标识)
// @param  "reserve" (保留异步任务对象 false:执行完成后删除项)
// @return 完成标志(true:任务完成)
bool TaskAsyncPromise(
	std::unordered_map<std::string, std::shared_future<void>>& input,
	strkey                                                     itemkey, 
	const bool&                                                reserve
);
```
---

### 框架反射
> 反射只适用于用户注册类对象

反射类提供全局对象进行操作.
```cpp
CoreModuleSystem::ProfxReflection IMFXREF;
```

用户类必须继承框架接口才能使用反射.
[用户类注册](improfx_usage.md)
```cpp
// 框架接口(具体参见"用户类注册"文档):
class INTERFACE_PROFX_USER {
	public:
		// init: return flag: false:failed, true:success.
		virtual bool LogicInitialization(FRMCORE_PACKAGE& FrmDat) = 0;
		// loop: return flag: false:continue, true:exit.
		virtual bool LogicEventLoop(FRMCORE_PACKAGE& FrmDat, FRMCORE_INFO& FrmInfo) = 0;
		virtual void LogicCloseFree() = 0;
};

// 继承框架接口"CoreModuleIMFX::INTERFACE_PROFX_USER"
class UserClass :public CoreModuleIMFX::INTERFACE_PROFX_USER {...};
```

注册用户类, 使用反射 创建, 获取, 销毁 对象:
```cpp
// 通过宏注册反射类(类似UE但是没那么智能)
FXUCLASS(UserClass);

// 通过类名创建注册全局唯一对象
// 前者为以上注册的类名, 后者为唯一对象名(Key)
IMFXREF.ReflectionObjectCreate("UserClass", "UserClassObject");

// 删除全局唯一对象(不需要类名)
IMFXREF.ReflectionObjectDelete("UserClassObject");

// 获取全局唯一对象(通过RTTI转换为用户类)
UserClass* MyObj = IMFXREF.ReflectionObjectGet<UserClass>("UserClassObject");
```
---

### 核心静态函数
> 由核心类提供的静态工具, std::function包装调用

读取解析.json文件, 使用RapidJSON库, 使用时需要熟悉此库操作.
```cpp
// @param  const std::string& (文件路径名称), bool& (文件状态,true:读取成功,false:读取失败)
std::function<rapidjson::Document(const std::string&, bool&)> JsonFileLoader;

// 调用Demo:
bool temp_filestate = false;
rapidjson::Document MyDoc = FrmDat.JsonFileLoader("xx/xxx.json", temp_filestate);
```

---

- 相较于之前增加了许多新功能
- 在附着渲染纹理(特效)不可见的情况下尽量把该着色器暂停运行以减轻GPU负担
- 合理设置渲染条件和着色器最大帧率也能得到比较好的运行速度

 > 目前没用纹理合并所以因为纹理采样器限制, 传入着色器处理的纹理(占用TMU资源)数量通过 GL_MAX_TEXTURE_IMAGE_UNITS 决定

```END```
