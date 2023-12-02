# ImProFx - DyInfo
RCSZ

>- 2023_11_21 version 2.0.0 TEST

框架动态信息.

## 2.0.0T
参考: [使用文档](improfx_usage.md) 中事件循环接口中的参数: "FRMCORE_INFO& FrmInfo"

获取信息:
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

// 获取范围内均匀分布随机数(float)
float value = FrmInfo.GetFloatRandom(float min, float max);

// 获取注册用户对象信息
std::vector<regobjectinfo> objinfo = FrmInfo.USER_REGISTER_INFO_NAME;

struct regobjectinfo {
	std::string ClassName;     // 注册类名
	std::string UniqueObjName; // 注册全局唯一对象名(Key)
}
```

---

- 还在完善中...

Update 20231121
