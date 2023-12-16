# ImProFx - CoreFX
- [x] Update.20231216 RCSZ

[返回使用文档](improfx_usage.md)
- 所在目录: src/imgui_profx_src/improfx_core/

> 2023_11_02 version 1.0.0 Alpha
> 2023_11_05 version 1.1.0 Alpha
> 2023_11_21 version 2.0.0 Test-1
> 2023_12_02 version 2.0.0 Test-2
> 2023_12_08 version 2.0.0 Alpha
> 2023_12_16 version 2.1.0 Alpha

CoreFX为框架核心部分.

```cpp
class CoreModuleImGui::RenderImGui;   // 框架ImGui初始化
class CoreModuleWindow::RenderWindow; // 框架GLFW窗口
class FrameworkImCore;                // 框架核心
class ProfxSystemFramework;           // 框架启动
class ProfxReflection;                // 框架反射(200T)
```

OpenGL4.6图形底层(着色器)调用封装定义: src\imgui_profx_src\improfx_shader\

## 2.1.0A
> ShaderFx的调用, 工具的调用, 反射使用

__请先阅读配置文件文档:__ [配置文件文档](improfx_config.md)

## 调用&操作核心工具
> namespace ImProFxTools

通过全局对象调用工具(以下为定义):
```cpp
ImProFxTools::FrameworkCoreFind    ImProFxTools::FrmCore;     // 核心数据查找(查找调用)
ImProFxTools::FrameworkCoreManager ImProFxTools::FrmCoreOper; // 核心数据操作(动态创建删除)
ImProFxTools::FrameworkStaticFind  ImProFxTools::FrmStatic;   // 静态资源(查找&分配&删除静态资源[指在框架中保存的资源])
ImProFxTools::FrameworkTaskFind    ImProFxTools::FrmTask;     // 异步任务(创建获取异步[非多线程]结果)
```

### ImProFxTools::FrmCore
> 直接通过全局对象调用以下工具 ImProFxTools::FrmCore.Find...

着色器包结构体定义"FxRenderItem"
```cpp
struct FxRenderItem {

	FsModelHanlde ModelHandle; // OpenGL模型句柄(VAO,VBO)

	uint32_t ShaderProgramHandle; // OpenGL着色器程序句柄
	uint32_t FrameBufferHandle;   // OpenGL帧缓冲句柄(FBO)

	FsTextureHandle FrameBufferTex; // FBO附着纹理的句柄

	float     RenderTimer;     // 着色器渲染计时器(计数器)
	FsMatrix4 RenderMatrixMvp; // 着色器渲染MVP矩阵

	std::chrono::steady_clock::time_point RenderRateTimer; // 着色器帧率限制计时器
	int64_t                               RenderRateTime;  // 着色器帧率限制时间

	std::vector<FsTextureHandle>           ParamTexture;    // 着色器资源纹理句柄数组
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
// @param  "input" (FrmDat.DataShaderFx), "itemkey" (Key包名称)
// @return OpenGL纹理句柄
uint32_t FindRenderItemFBO(std::unordered_map<std::string, FxRenderItem>& input, const char* itemkey);
```

查询设置渲染项(着色器包), 自定义(着色器额外参数)"Uniform"变量float.
```cpp
// @param "input" (FrmDat.DataShaderFx), "itemkey" (Key包名称)
// @param "ufn" (变量名称"shader_uniform_param"配置项), "value" (设置值)
void FindSetRenderItemParam(
    std::unordered_map<std::string, FxRenderItem>& input, 
    const char*                                    itemkey, 
    const char*                                    ufn, 
    float                                          value
);
```

查询获取设置渲染项(着色器包), 渲染状态(着色器运行标志).
```cpp
// @param "input" (FrmDat.DataShaderFx), "itemkey" (Key包名称)
// @param "flag" (设置状态 true:运行, false:暂停)
void FindSetRenderItemFlag(std::unordered_map<std::string, FxRenderItem>& input, const char* itemkey, bool flag);
```

查询获取Raw纹理项("core_raw_texture_config"配置项).
```cpp
// @param  "input" (FrmDat.DataTextureRaw), "itemkey" (Key纹理名称"image_unique_name"配置项)
// @return OpenGL纹理句柄
uint32_t FindRawTextureImg(std::unordered_map<std::string, FStextureHandle>& input, const char* itemkey);
```

__简单Demo(框架内注册用户类中获取着色器FBO和源纹理句柄):__
```cpp
FrmCore.FindRenderItemFBO(FrmDat.DataShaderFx, "MyShader");
FrmCore.FindRawTextureImg(FrmDat.DataTextureRaw, "MyTexture");
```
---

### ImProFxTools::FrmCoreOper
> 直接通过全局对象调用以下工具 ImProFxTools::FrmCoreOper.Find...

着色器包动态加载器"ImProFxTools::FxShaderLoader"
__目前不建议使用__
```cpp
// 定义:
class ImProFxTools::FxShaderLoader;

// 构造函数:
// "script" (片元着色器代码) 只需要编写片元着色器
// 并且只需要编写GLSL中的 void main() {...} 部分
FxShaderLoader(const std::string& script);
// "vert" (顶点着色器代码), "frag" (片元着色器代码)
// 编写完整着色器代码
FxShaderLoader(const std::string& vert, const std::string& frag);

// 编译着色器&获取着色器程序句柄:
uint32_t FxShaderProgramCreateCompile();

// 配置着色器包:
// @param "vertexhandle" (模型句柄, x:VAO y:VBO), "resolution" (渲染分辨率), "rate" (最大渲染帧率)
// @param "render_scale" (渲染缩放比例)
void FxShaderProgramConfig(
	const Vector2T<uint32_t>& vertexhandle,
	const Vector2T<float>&    resolution,
	const float&              rate,
	const Vector2T<float>&    render_scale = Vector2T<float>(1.0f, 1.0f)
);

// 着色器包资源&参数加载:
// @param "uniform_texture" (纹理句柄), "uniform_float" (浮点数参数)
void FxShaderParamLoader(
	const std::vector<FsTextureHandle>&           uniform_texture,
	const std::unordered_map<std::string, float>& uniform_float
);
```

查询动态加载渲染项(着色器包): 
__目前不建议使用__
> 因为涉及到手动分配 VAO,VBO 以及纹理加载, 非常非常麻烦.
```cpp
// @param  "input" (FrmDat.DataShaderFx), "itemkey" (Key包名称), "loader" FxShader加载器(见以上讲解)
// @return true:加载成功, false:加载失败
bool FindLoadRenderItem(std::unordered_map<std::string, FxRenderItem>& input, strkey itemkey, FxShaderLoader& loader);
```

查询删除渲染项(着色器包):
```cpp
// @param  "input" (FrmDat.DataShaderFx), "itemkey" (Key包名称)
// @return true:删除成功, false:删除成功
void FindDeleteRenderItem(std::unordered_map<std::string, FxRenderItem>& input, strkey itemkey);
```

查询动态加载Raw纹理项(像素源数据):
```cpp
struct SrcImage {
	uint32_t ImageWidth, ImageHeight, ImageChannels; // 图片 宽度&高度&颜色通道数
	uint8_t* ImagePixelPointer;                      // 图片像素数据指针
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

查询删除Raw纹理项:
```cpp
// @param  "input" (FrmDat.DataTextureRaw), "itemkey" (Key纹理名称)
// @return true:删除成功, false:删除失败
bool FindDeleteRawTextureImg(std::unordered_map<std::string, FsTextureHandle>& input, strkey itemkey);
```

__简单Demo(框架内注册用户类中删除着色器包和源纹理资源):__
```cpp
bool state = FrmCoreOper.FindDeleteRenderItem(FrmDat.DataShaderFx, "MyShader");
bool state = FrmCoreOper.FindDeleteRawTextureImg(FrmDat.DataTextureRaw, "MyTexture");
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
// @return true:删除成功, false:删除失败
bool StaticFlagDelete(std::unordered_map<std::string, bool>& input, const char* itemkey, deletemode mode = STCMODE_DELETE);
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
// @return true:删除成功, false:删除失败
bool StaticTimerDelete(std::unordered_map<std::string, ChronoTimer*>& input, strkey itemkey, deletemode mode = STCMODE_DELETE);
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
// @return true:删除成功, false:删除失败
bool StaticParticleSysDelete(std::unordered_map<std::string, IFC_PARTC::ParticleBase*>& input, strkey itemkey, deletemode mode = STCMODE_DELETE);
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
void AsyncTaskCreate(std::unordered_map<std::string, std::shared_future<void>>& input, strkey itemkey, std::future<void> async);
```

等待异步任务完成(暂时无返回值)
```cpp
// @param  "input" (FrmDat.TaskAsync), "itemkey" (Key标识)
// @param  "reserve" (保留异步任务对象 false:执行完成后删除项)
// @return 完成标志(true:任务完成)
bool AsyncTaskPromise(
	std::unordered_map<std::string, std::shared_future<void>>& input,
	strkey                                                     itemkey, 
	const bool&                                                reserve
);
```
---

### 框架反射
> 反射只适用于用户注册类对象

反射类提供全局对象进行操作:
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
> 由框架核心提供的静态工具, std::function包装调用

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