# ImProFx - DyInfo
- [x] Update.20240110 RCSZ

框架动态信息.

## 2.2.0A
获取使用参考: [使用文档](improfx_usage.md) 中事件循环接口中的参数: "FRMCORE_INFO& FrmInfo"

获取信息(以Demo形式讲解说明):
```cpp
// 获取GLFW窗口大小xy
Vector2T<float> size = FrmInfo.WindowSize;

// 获取渲染速率缩放(基准60fps)
float value = FrmInfo.FrameSmoothValue;

// 获取鼠标位置xy
Vector2T<float> ms_pos = FrmInfo.MousePosition;

// 获取GLFW窗口聚焦标志
bool flag = FrmInfo.WindowFoucs;

// 当文件(s)拖入GLFW窗口, 获取所有文件路径
std::vector<std::string>* files = FrmInfo.DropFiles;
// 获取完成清理当前所有文件路径缓存
FrmInfo.DropFilesOK();

// 获取单帧内唯一标识符(一般用于ImGui::PushID)
uint32_t id = FrmInfo.GetUniqueID();

// 框架内ImGui函数封装
// 目前封装的控件均有每帧UniqueID不需要担心ImGui冲突
FrmInfo.ImProFXGUI->Text("");
FrmInfo.ImProFXGUI->...

// 获取范围内均匀分布随机数(float)
float value = FrmInfo.GetFloatRandom(float min, float max);

// 获取注册用户对象信息
std::vector<regobjectinfo> objinfo = FrmInfo.USER_REGISTER_INFO_NAME;

struct regobjectinfo {
	std::string ClassName;     // 注册类名
	std::string UniqueObjName; // 注册全局唯一对象名(Key)
}
```

框架内部调试开发:
```cpp
// 调试开发用 句柄 & 无类型指针.
uint32_t devehd = FrmInfo._DeveloperTestHandle;
void* deveptr = FrmInfo._DeveloperTestPointer;
```

---

- 持续完善开发中...

>- 2023_11_21 version 2.0.0 Test1
>- 2023_12_02 version 2.0.0 Test2
>- 2023_12_08 version 2.0.0 Alpha
>- 2023_12_16 version 2.1.0 Alpha
>- 2024_01_10 version 2.2.0 Alpha

```END```