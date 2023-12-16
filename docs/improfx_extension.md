# ImProFx - ImGuiPro
- [x] Update.20231216 RCSZ

[返回使用文档](improfx_usage.md)
- 所在目录: src/imgui_profx_src/framework_extension/

> 2023_12_16 version 2.1.0 Alpha

## 2.1.0A
> 框架扩展 模块&控件&第三方库

### 框架标准扩展
> 全部封装以类的方式提供调用

__ext_fileloader__ 文件加载器
```cpp
// 二进制文件加载器
class FileLoaderBinary;

// 读取二进制文件(使用构造函数)
// @param "filename" (文件路径)
FileLoaderBinary(const std::string& filename);
FileLoaderBinary() {};

// 二进制写入文件
// @param "filename" (文件路径) "databin" (字节数据) "mode" (模式)
bool WriterFileBinary(
    const std::string&          filename, 
    const std::vector<uint8_t>& databin, 
    std::ios_base::openmode     mode = std::ios_base::out
);

// 获取读取的二进制文件数据
std::vector<uint8_t> GetDataBinary();
// 获取文件(数据)大小
size_t GetFileSize();

// 文本文件加载器
// 与以上操作方式一样, 只不过类型为 std::string
class FileLoaderString;

FileLoaderString(const std::string& filename);
FileLoaderString() {};

bool WriterFileString(const std::string& filename, const std::string& databin, std::ios_base::openmode mode = std::ios_base::out);

std::string GetDataString();
size_t GetFileSize();
```
---

__ext_imguicontrol__ ImGui框架扩展控件
> 包含第三方扩展控件以及自制控件

第三方控件扩展(引入后全部封装为类)

- knobs: https://github.com/altschuler/imgui-knobs
- memory_editor: https://github.com/ocornut/imgui_club

```cpp
// 旋钮扩展控件, 这里把两种形式(int,float)封装到一起了
class ImExtKnobs;

int* GetIntPointer();     // 获取Int指针
float* GetFloatPointer(); // 获取Float指针

enum ImExtKnobsType {
	KnobInt   = 1 << 1, // Int形式执行
	KnobFloat = 1 << 2  // Float形式执行
};

bool DrawKnobs(
	ImExtKnobsType   type,             // 选择类型
	const char*      name,             // 控件名称
	const ImVec2&    limit,            // 值限制 x:min, y:max
	float            speed   = 0.0f,   // 速度
	const char*      format  = "%.2f", // 格式化小数
	ImGuiKnobVariant variant = 1,      // 样式
	float            size    = 0.0f,   // 大小
	ImGuiKnobFlags   flags   = 0,      // 标志
	int              steps   = 10      // 步进
)

// 内存编辑器窗口扩展, 这个好像是由ImGui官方维护的
class ImExtMemoryEditor;

// 获取内存(vector)指针
std::vector<T>* GetMemoryPointer();

// 绘制内存编辑器 "name" (控件名称)
void DrawMemoryEditor(const char* name)
```
---

ImProFX 扩展动画控件 非第三方(与动画模块不同, 此提供更定制化复杂的动画控件)
> namespace ImExtAnim

```cpp
// 动画菜单控件
// 其中两个选择框, 一个跟随鼠标移动, 一个固定为当前选项, 固定选择框颜色是根据全局ImGui字体颜色计算
class AnimMenuChildWindow;

// @param "name" (控件名称) "items" (菜单选项) "count" (菜单选择计数, 对应选项)
// @param "color" (跟随选择框颜色) "size" (子窗口大小) "textscale" (菜单文本缩放)
void DrawMenuWindow(
	const char*                     name,
	const std::vector<std::string>& items,
	uint32_t&                       count,
	const ImVec4&                   color     = ImVec4(0.55f, 0.12f, 1.0f, 0.72f),
	const ImVec2&                   size      = ImVec2(256.0f, 384.0f),
	float                           textscale = 1.2f
)
```
---

__框架扩展开发__

- 引入 framework_extension_tools.hpp 头文件
- 把扩展的头文件引入 framework_extension_header.h 头文件

扩展使用的日志函数和框架内不一样(但是用法差不多):
```cpp
enum EXTLOGLABEL {
	LogError   = 1 << 1, // 错误
	LogWarning = 1 << 2, // 警告
	LogInfo    = 1 << 3, // 信息
	LogTrace   = 1 << 4, // 跟踪
	LogPerf    = 1 << 5  // 性能
};

// @param "Label" (日志标签) "ModuleLabel" (模块标签) "LogText" (日志格式化字符串) "..." (参数)
ExtLog::ELog(const EXTLOGLABEL& Label, const char* ModuleLabel, const char* LogText, ...);
```
---

- 目前开发重心在此, 会开发和引入更多好用的控件
- 完善框架扩展体系

```END```