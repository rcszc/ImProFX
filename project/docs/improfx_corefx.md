# ImProFx - CoreFX
- [x] Update.20240110 RCSZ

[返回使用文档](improfx_usage.md)
- 所在目录: src/imgui_profx_src/improfx_core/

__CoreFX为框架核心部分__

```cpp
class CoreModuleImGui::RenderImGui;   // 框架ImGui初始化
class CoreModuleWindow::RenderWindow; // 框架GLFW窗口
class FrameworkImCore;                // 框架核心
class ProfxSystemFramework;           // 框架启动
class ProfxReflection;                // 框架反射(200T)
```

OpenGL4.6图形抽象层, 图形库操作, 底层资源(LLRES)定义: src\imgui_profx_src\improfx_renderer\

## 2.2.0A
> ShaderFx的调用, 工具的调用, 反射使用

__请先阅读配置文件文档:__ [配置文件文档](improfx_config.md)

## 调用&操作核心工具
> namespace ImProFxTools

通过全局对象调用工具(以下为定义):
```cpp
ImProFxTools::FrameworkCoreFind    ImProFxTools::FrmCore;     // 核心数据查找(查找调用)
ImProFxTools::FrameworkCoreManager ImProFxTools::FrmCoreOper; // 核心数据操作(动态创建删除)
ImProFxTools::FrameworkStaticFind  ImProFxTools::FrmStatic;   // 静态资源(查找&分配&删除静态资源[指在框架中保留状态的资源])
ImProFxTools::FrameworkTaskFind    ImProFxTools::FrmTask;     // 异步任务(创建获取异步[非多线程]结果)
```

### ImProFxTools::FrmCore
> 直接通过全局对象调用以下工具: ImProFxTools::FrmCore.Find...

FX着色器结构体定义"FxRenderItem"
```cpp
struct FxRenderItem {

	std::string ShaderProgramItem; // 着色器程序
	std::string FrameBufferItem;   // 帧缓冲(FBO)

	std::string ModelItem;          // 渲染模型(VAO,VBO)
	std::string FrameBufferTexItem; // 帧缓冲附着纹理

	float     RenderTimer;     // 渲染计时器(计数器)虚拟Tick
	FsMatrix4 RenderMatrixMvp; // 渲染MVP矩阵

	std::chrono::steady_clock::time_point RenderRateTimer; // 渲染帧率计时器
	int64_t                               RenderRateTime;  // 渲染帧率

	std::vector<std::string>               ParamTextureItems; // 着色器采样纹理
	std::unordered_map<std::string, float> ParamValueFloat;   // 着色器Uniform参数(fp32)

	bool ExecuteProgramFlag; // 着色器程序运行标志
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
> 直接通过全局对象调用以下工具: ImProFxTools::FrmCoreOper.Find...

~~着色器包动态加载器"ImProFxTools::FxShaderLoader"~~
...
~~查询动态加载渲染项(着色器包):~~
...
~~查询删除渲染项(着色器包):~~
...
> 因为重构渲染抽象层以及底层资源管理, 暂时不考虑动态编译加载着色器

查询动态加载Raw纹理项(像素源数据):
```cpp
struct SrcImage {
	uint32_t ImageWidth, ImageHeight, ImageChannels; // 图片 宽度&高度&颜色通道数
	uint8_t* ImagePixelsPointer;                      // 图片像素数据指针
};

// @param  "input" (FrmDat.DataTextureRaw), "itemkey" (Key纹理名称), "data" 图片源数据
// @return true:加载成功, false:加载失败
bool FindLoadRawTextureImgData(std::unordered_map<std::string, std::string>& input, strkey itemkey, const SrcImage& data);
```

查询动态加载Raw纹理项(图片文件):
```cpp
// @param  "input" (FrmDat.DataTextureRaw), "itemkey" (Key纹理名称), "file" 图片文件路径(使用stb_image加载)
// @return true:加载成功, false:加载失败
bool FindLoadRawTextureImgFile(std::unordered_map<std::string, std::string>& input, strkey itemkey, const std::string& file);
```

查询删除Raw纹理项:
```cpp
// @param  "input" (FrmDat.DataTextureRaw), "itemkey" (Key纹理名称)
// @return true:删除成功, false:删除失败
bool FindDeleteRawTextureImg(std::unordered_map<std::string, std::string>& input, strkey itemkey);
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

- 相较于之前增加了许多新功能.
- 在附着渲染纹理(特效)不可见的情况下尽量把该着色器暂停运行以减轻GPU负担.
- 合理设置渲染条件和着色器最大帧率也能得到比较好的运行速度.
- 目前没设计纹理合并所以因为纹理采样器限制, 传入着色器处理的纹理(占用TMU资源)数量通过 GL_MAX_TEXTURE_IMAGE_UNITS 决定.

>- 2023_11_02 version 1.0.0 Alpha
>- 2023_11_05 version 1.1.0 Alpha
>- 2023_11_21 version 2.0.0 Test1
>- 2023_12_02 version 2.0.0 Test2
>- 2023_12_08 version 2.0.0 Alpha
>- 2023_12_16 version 2.1.0 Alpha
>- 2024_01_10 version 2.2.0 Alpha

```END```