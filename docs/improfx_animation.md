# ImProFx - Animation
RCSZ

- 所在目录: src/imgui_profx_src/improfx_anim/

> 2023_11_02 version 1.0.0 ahpla

|动画控件Class结构| 版本 |
|----|----|
|ImAnimationBase -> ImAnimButton | 1.0.0A |
|ImAnimationBase -> ImAnimFixedWindow | 1.0.0A |

## 1.0.0A
> 框架属于刚捏出形状以后慢慢加控件 /滑稽

namespace ImGuiProAnim

注册控件对象使用"AnimCompReg", "input"输入"DataCompAnim"这是框架内储存动画对象信息的HashMap, "key" = HashMap.key(Unique), "comp"输入需要注册的控件对象.
```cpp
void ImGuiProAnim::AnimCompReg(
    std::unordered_map<std::string, ImAnimationBase*>& input, 
    const char* key, 
    ImAnimationBase* comp
);

// Demo:
ImGuiProAnim::AnimCompReg(DataCompAnim, "ButtonA", Object);
```
- 所有控件在调用"CreateAnim..."的时候都会配置默认参数.

---

### ImAnimButton
imgui扩展按钮动画控件类, 调整按钮3种状态的 颜色 & 大小; 状态为normal(常规), hover(点击,鼠标放在控件上), active(激活,鼠标放在控件上并左键).
```cpp
class ImGuiProAnim::ImAnimButton();
```

控件的创建和配置(Init阶段), 创建控件对象使用"CreateAnimButton", 配置的两组参数有 color & size, 见以下注释.
```cpp
// "copy"是当已有一个配置好的对象可以复制参数创建.
ImAnimButton* ImGuiProAnim::CreateAnimButton(ImAnimButton* copy = nullptr);

auto demo = ImGuiProAnim::CreateAnimButton();

// 参数: Vector4T<float>
demo->config_normal_color;  // 常规状态颜色.
demo->config_hover_color;   // 点击状态颜色.
demo->config_active_color;  // 激活状态颜色.

// 参数:  Vector2T<float>
demo->config_normal_size; // 常规状态大小.
demo->config_hover_size;  // 点击状态大小.
demo->config_active_size; // 激活状态大小.
```

平滑差值系数(默认为1.0, 不常用配置参数).
```cpp
demo->config_color_transspeed;
demo->config_size_transspeed;
```

控件的调用(Loop阶段), "CallAnimButton"和"CallAnimButtonImage", 前者为按钮控件对应"ImGui::Button", 后者为图片(纹理)按钮对应"ImGui::ImageButton"具体前往源码.
```cpp
// 返回按钮状态标志.
bool CallAnimButton(
	const char*                                        unique_name, // 按钮名称(全局唯一参考ImGui).
	std::unordered_map<std::string, ImAnimationBase*>& input,       // 输入"DataCompAnim".
	const char*                                        comp_name,   // 控件的注册名称(key, 参考以上"AnimCompReg").
	float                                              trans_smooth = 1.0f // 平滑缩放参数.
);

// 返回按钮状态标志.
// unique_name ~ comp_name 和 trans_smooth 跟以上注释功能相同.
bool CallAnimButtonImage(
	const char*                                        unique_name,
	std::unordered_map<std::string, ImAnimationBase*>& input,
	const char*                                        comp_name,
	uint32_t                                           texture_hd, // OpenGL纹理句柄.
	float                                              trans_smooth = 1.0f
);
```
---

### ImAnimFixedWindow
imgui扩展固定窗口动画控件类, 调整窗口2种状态的 颜色 & 大小; 子窗口状态为close(关闭), open(打开).
```cpp
class ImGuiProAnim::ImAnimFixedWindow();
```

控件的创建和配置(Init阶段), 创建控件对象使用"CreateAnimFixedWindow", 配置的两组参数有 color & size, 见以下注释.
```cpp
// "copy"是当已有一个配置好的对象可以复制参数创建.
ImAnimFixedWindow* ImGuiProAnim::CreateAnimFixedWindow(ImAnimFixedWindow* copy = nullptr);

auto demo = ImGuiProAnim::CreateAnimFixedWindow();

// 参数: Vector4T<float>
demo->config_open_color;  // 子窗口打开状态背景颜色.
demo->config_close_color; // 子窗口关闭状态背景颜色(虽然不可见主要与打开状态产生过渡).

// 参数:  Vector2T<float>
demo->config_open_size;  // 子窗口打开状态固定大小.
demo->config_close_size; // 子窗口关闭状态大小(虽然不可见主要与打开状态产生过渡).
```

平滑差值系数(默认为1.0, 不常用配置参数).
```cpp
demo->config_color_transspeed;
demo->config_size_transspeed;
```

控件的调用(Loop阶段), "CallAnimFixedWindow", 对应"ImGui::Begin - ImGui::End", 通过"std::function<void()>"包装.
```cpp
void CallAnimFixedWindow(
	const char*                                        unique_name, // 子窗口名称(全局唯一参考ImGui).
	std::unordered_map<std::string, ImAnimationBase*>& input,       // 输入"DataCompAnim".
	const char*                                        comp_name,   // 控件的注册名称(key, 参考以上"AnimCompReg").
	bool&                                              open_flag,   // 窗口状态(参考ImGui::Begin的bool* p_open标志).
	std::function<void()>                              window_ui,   // 子窗口内控件包装.
	ImGuiWindowFlagsAm                                 flags = 0,   // 窗口标志(等与ImGui::Begin的ImGuiWindowFlags Flags).
	float                                              trans_smooth = 1.0f // 平滑缩放
);

// Demo:
ImGuiProAnim::CallAnimFixedWindow("Win", DataCompAnim, "CompWindow", window_flag,
	[&]() {
	    // 子窗口内 ImGui 控件.
	}
);
```

---

动画控件~~正常情况下~~有自动回收不需要额外的释放, 动画对象存储在框架 src\imgui_profx_src\improfx_core\framework_render.h 中58行定义, framework_render_system.cpp 文件中273行释放.
```cpp
std::unordered_map<std::string, ImGuiProAnim::ImAnimationBase*> CoreModuleRender::FrameworkRender::DataCompAnim = {};
```

- Update 20231102