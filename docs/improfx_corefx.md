# ImProFx - CoreFX
RCSZ

[返回使用文档](improfx_usage.md)
- 所在目录: src\imgui_profx_src\improfx_core\

> 2023_11_02 version 1.0.0 ahpla
> 2023_11_05 version 1.1.0 ahpla
> 2023_11_21 version 2.0.0 TEST

CoreFX为框架核心部分.

```cpp
class CoreModuleImGui::RenderImGui;   // 框架ImGui初始化
class CoreModuleWindow::RenderWindow; // 框架GLFW窗口
class FrameworkImCore;                // 框架核心
class ProfxSystemFramework;           // 框架启动
class ProfxReflection;                // 框架反射(200T)
```

GLEW着色器调用封装定义: src\imgui_profx_src\improfx_shader\

## 2.0.0T
> 配置文件, ShaderFx的调用, 工具的调用, 反射使用

### 配置文件结构
配置文件使用.json文件, 使用RapidJSON库解析, 通常此框架需要两个必要核心配置文件"window_config.json"和"shader_config.json", 前者为配置GLFW窗口和ImGui的初始化等, 后者为配置加载OpenGL着色器文件&资源; 加载顺序为 window_config => shader_config.

"window_config.json"具体参加注释:
```cpp
{
  "window_name": "ImProFX - V2.0 TEST",                 // GLFW窗口标题
  "window_size": [ 1680, 945 ],                         // GLFW窗口大小[ width,height ]
  "window_MSAA": 4,                                     // GLFW抗锯齿(MSAA)等级
  "window_fw_version": [ 3, 3 ],                        // GLFW版本配置 x.x
  "window_sh_version": "#version 330 core",             // ImGui配置着色器版本
  "imgui_fonts_file": "imgui_profx_system/Dengb.ttf",   // ImGui加载字体(全局)
  "imgui_fonts_size": 32.0,                             // ImGui配置字体大小(全局)
  "imgui_fonts_color": [ 0.0, 1.0, 0.85, 1.0 ],         // ImGui配置字体颜色(全局)
  "imgui_window_rounding": 3.2,                         // ImGui窗口圆角程度
  "imgui_frame_rounding": 5.5,                          // ImGui帧控件圆角程度
  "shader_reg_file": "imgui_profx_system/config/shader_config.json" // 着色器配置文件路径
}
```

"shader_config.json"具体参加注释(文件路径对应以上"shader_reg_file"配置项):
```cpp
{
  "core_shaderfx_config": [ // 着色器包(package)数组
    {
      "shaderfx_package": "aaaxxx",                               // 包名称Key(Unique)
      "shader_file_vertex": "imgui_profx_system/shader/xxx.vs",   // 顶点着色器代码文件路径
      "shader_file_fragment": "imgui_profx_system/shader/xxx.fs", // 片元着色器代码文件路径
      "shader_fbo_texture_name": "xxx",                           // fbo附着的纹理名称(Unique)
      "matrix_ortho_cube": 10.0,                                  // 2D正交视图矩阵缩放(当前着色器程序[包])
      "render_max_fps": 200,                                      // 当前着色器渲染最大帧率(微秒级计时器)
      "render_max_resolution": [ 1024,576 ],                      // 当前着色器渲染分辨率(如果为[ -1,-1 ]那么默认为窗口分辨率相同) 
      "shader_uniform_texture": [ // 着色器采样纹理(通过Uniform传入着色器的纹理)
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
```cpp
// framework_core.hpp
std::unordered_map<std::string, FStextureHandle> CoreModuleIMFX::FRMCORE_PACKAGE::DataRawTexture;
```

简略流程: 加载着色器项(包)(每个包内一个独立着色器程序) -> 每个包通过FBO附着渲染到附着纹理"shader_fbo_texture_name"(非屏幕) -> 查询搜索对应包 -> 通过ImGui调用纹理句柄渲染到GUI界面(屏幕).

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
ImProFxTools::FrameworkDataFind   ImProFxTools::FrmData;   // 数据处理(查找修改处理着色器资源和源贴图资源)
ImProFxTools::FrameworkStaticFind ImProFxTools::FrmStatic; // 静态资源(申请查找删除静态[?]资源)
ImProFxTools::FrameworkTaskFind   ImProFxTools::FrmTask;   // 异步任务(创建获取异步任务[非多线程]和结果)
```

### ImProFxTools::FrmData
> 直接通过全局对象调用以下函数 ImProFxTools::FrmData.Find...

着色器包结构体定义"FxRenderItem":
```cpp
struct FxRenderItem {

	FSmodelHanlde ModelHandle; // OpenGL模型句柄(VAO,VBO)

	FSGLuint ShaderProgramHandle; // OpenGL着色器程序句柄
	FSGLuint FrameBufferHandle;   // OpenGL帧缓冲句柄(FBO)

	FStextureHandle FrameBufferTex; // FBO附着纹理的句柄

	float     RenderTimer;     // 着色器渲染计时器(计数器)
	FSmatrix4 RenderMVPmatrix; // 着色器渲染MVP矩阵

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

查询获取渲染项(着色器包)FBO附着纹理(着色器渲染完成的图像).
```cpp
// @param  "input" (FrmDat.DataShaderFx), "itemkey" (Key包名称"shaderfx_package"配置项)
// @return OpenGL纹理句柄
uint32_t FindRenderItemFBO(std::unordered_map<std::string, FxRenderItem>& input, const char* itemkey);
```

查询获取设置渲染项(着色器包)自定义(着色器额外参数)"Uniform"变量float.
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

查询获取设置渲染项(着色器包)渲染状态(着色器运行标志).
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
void FindLoadRawTextureImgData(std::unordered_map<std::string, FStextureHandle>& input, strkey itemkey, const SrcImage& data);
```

查询动态加载Raw纹理项(图片文件):
```cpp
// @param  "input" (FrmDat.DataTextureRaw), "itemkey" (Key纹理名称), "file" 图片文件路径(使用stb_image加载)
void FindLoadRawTextureImgFile(std::unordered_map<std::string, FStextureHandle>& input, strkey itemkey, const std::string& file);
```

__简单Demo(框架内注册用户类中获取着色器FBO和源纹理句柄):__
```cpp
FrmData.FindRenderItemFBO(FrmDat.DataShaderFx, "MyShader");
FrmData.FindRawTextureImg(FrmDat.DataTextureRaw, "MyTexture");
```
---

### ImProFxTools::FrmStatic
> 直接通过全局对象调用以下函数 ImProFxTools::FrmStatic.Static...

申请静态标志(bool), 可在Init和Loop部分.
```cpp
// @param "input" (FrmDat.StaticFlag), "itemkey" (Key标识), "init" (初始化标志)
// @param "REPEAT" (查重 true:如果遇到相同"text"[Key]则不覆盖写入)
void StaticFlagAlloc(std::unordered_map<std::string, bool>& input, const char* itemkey, bool init = false, bool REPEAT = false);
```

删除静态标志(bool) delete item(key-value).
```cpp
// @param "input" (FrmDat.StaticFlag), "itemkey" (Key标识)
void StaticFlagDelete(std::unordered_map<std::string, bool>& input, const char* itemkey);
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
// @param "input" (FrmDat.StaticTimer), "itemkey" (Key标识)
void StaticTimerAlloc(std::unordered_map<std::string, ChronoTimer*>& input, strkey itemkey);
```

删除(释放)静态计时器对象 delete object & item(key-value).
```cpp
// @param "input" (FrmDat.StaticTimer), "itemkey" (Key标识)
void StaticTimerDelete(std::unordered_map<std::string, ChronoTimer*>& input, strkey itemkey);
```

查询获取静态计时器对象指针.
```cpp
// @param "input" (FrmDat.StaticTimer), "itemkey" (Key标识)
// @return object_pointer
ChronoTimer* StaticTimerFind(std::unordered_map<std::string, ChronoTimer*>& input, strkey itemkey);
```

__简单Demo(框架内注册用户类中调用静态标志和静态计时器):__
```cpp
auto MyFlag = FrmStatic.StaticFlagFind(FrmDat.StaticFlag, "MyStaticFlag");
FrmStatic.StaticTimerFind(FrmDat.StaticTimer, "MyStaticTimer")->ContextTimerUse(...);
```
---

### ImProFxTools::FrmTask
> 直接通过全局对象调用以下函数 ImProFxTools::FrmTask.TaskAsync...
- 当前还在开发阶段, 异步任务无返回值(void) - 20231121

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

- 相较于1.x增加了许多新功能(但是同时也开了很多新坑)
- 在特效纹理(FBO texture)不可见的情况下尽量把该着色器暂停运行以减轻GPU负担
- 合理设置渲染条件和着色器最大帧率也能得到比较好的运行速度

 > 目前没用纹理合并所以因为纹理采样器限制, 全局传入着色器处理的纹理(占用TMU资源)数量通过 GL_MAX_TEXTURE_IMAGE_UNITS 决定

Update 20231121