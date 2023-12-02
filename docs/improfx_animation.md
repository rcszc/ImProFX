# ImProFx - Animation
RCSZ

[返回使用文档](improfx_usage.md)
- 所在目录: src/imgui_profx_src/improfx_anim/

>- 2023_11_02 version 1.0.0 alpha
>- 2023_11_05 version 1.1.0 ahpla
>- 2023_11_21 version 2.0.0 TEST

|动画控件Class结构| 版本 |
|----|----|
|ImAnimationBase -> ImAnimButton | 1.0.0A |
|ImAnimationBase -> ImAnimFixedWindow | 1.0.0A |
|ImAnimationBase -> ImAnimSrcMoveColor | 1.1.0A |
|NULL | 2.0.0T |

## 2.0.0T
> namespace ImGuiProAnim

插值动画对象定义.
```cpp
std::unordered_map<std::string, ImGuiProAnim::ImAnimationBase*> CoreModuleIMFX::FRMCORE_PACKAGE::DataCompAnim;
```

注册控件插值动画对象, 在框架生命周期结束时自动释放.
```cpp
// @param "input" (FrmDat.DataAnimation), "key" (Key标识), "comp" (需要注册的控件对象)
void ImGuiProAnim::AnimCompReg(
    std::unordered_map<std::string, ImAnimationBase*>& input, 
    const char*                                        key, 
    ImAnimationBase*                                   comp
);

// Demo:
ImGuiProAnim::AnimCompReg(DataCompAnim, "MyButton", Object);
```
- 所有控件在调用"CreateAnim..."的时候都会配置默认参数.

---

### ImAnimButton
imgui扩展按钮动画控件类, 调整按钮3种状态的颜色&大小; 状态为normal(常规), hover(点击,鼠标放在控件上), active(激活,鼠标放在控件上并左键).
```cpp
class ImGuiProAnim::ImAnimButton();
```

控件的创建和配置(Init阶段), 创建控件对象使用"CreateAnimButton", 可配置的两组参数有 color & size, 见以下注释.
```cpp
// "copy"是当已有一个配置好的对象可以复制参数创建.
ImAnimButton* ImGuiProAnim::CreateAnimButton(ImAnimButton* copy = nullptr);

auto demo = ImGuiProAnim::CreateAnimButton();

// 参数: Vector4T<float>
demo->config_normal_color; // 常规状态颜色
demo->config_hover_color;  // 点击状态颜色
demo->config_active_color; // 激活状态颜色

// 参数: Vector2T<float>
demo->config_normal_size; // 常规状态大小
demo->config_hover_size;  // 点击状态大小
demo->config_active_size; // 激活状态大小
```

平滑插值系数(默认为1.0, 不常用配置参数).
```cpp
demo->config_color_transspeed;
demo->config_size_transspeed;
```

控件的调用(Loop阶段), "CallAnimButton"和"CallAnimButtonImage", 前者为按钮控件对应"ImGui::Button", 后者为图片(纹理)按钮对应"ImGui::ImageButton".
```cpp
// @param  "unique" (按钮名称, 参考ImGui), "input" (FrmDat.DataAnimation)
// @param  "comp_name" (Key标识), "trans_smooth" (步进缩放)
// @return 按钮状态标志
bool CallAnimButton(
	const char*                                        unique_name,
	std::unordered_map<std::string, ImAnimationBase*>& input,
	const char*                                        comp_name,
	float                                              trans_smooth = 1.0f
);

// @param (unique_name ~ comp_name 和 trans_smooth 跟以上注释功能相同)
// @param  "texture_hd" (OpenGL纹理句柄)
// @return 按钮状态标志
bool CallAnimButtonImage(
	const char*                                        unique_name,
	std::unordered_map<std::string, ImAnimationBase*>& input,
	const char*                                        comp_name,
	uint32_t                                           texture_hd,
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
// "copy"是当已有一个配置好的对象可以复制参数创建
ImAnimFixedWindow* ImGuiProAnim::CreateAnimFixedWindow(ImAnimFixedWindow* copy = nullptr);

auto demo = ImGuiProAnim::CreateAnimFixedWindow();

// param: Vector4T<float>
demo->config_open_color;  // 子窗口打开状态背景颜色
demo->config_close_color; // 子窗口关闭状态背景颜色(虽然不可见主要与打开状态产生过渡)

// param:  Vector2T<float>
demo->config_open_size;  // 子窗口打开状态固定大小
demo->config_close_size; // 子窗口关闭状态大小(虽然不可见主要与打开状态产生过渡)
```

平滑插值系数(默认为1.0, 不常用配置参数).
```cpp
demo->config_color_transspeed;
demo->config_size_transspeed;
```

控件的调用(Loop阶段), "CallAnimFixedWindow", 对应"ImGui::Begin - ImGui::End", 通过"std::function<void()>"包装.
```cpp
// @param "unique" (按钮名称, 参考ImGui), "input" (FrmDat.DataAnimation)
// @param "comp_name" (Key标识), "open_flag" (窗口状态, 参考ImGui::Begin的bool* p_open标志)
// @param "window_ui" (窗口函数对象), "flags" (窗口标志, 与ImGui::Begin的ImGuiWindowFlags Flags等价)
// @param "trans_smooth" (步进缩放)
void CallAnimFixedWindow(
	const char*                                        unique_name, 
	std::unordered_map<std::string, ImAnimationBase*>& input,
	const char*                                        comp_name,
	bool&                                              open_flag,
	std::function<void()>                              window_ui,
	ImGuiWindowFlagsAm                                 flags = 0,
	float                                              trans_smooth = 1.0f
);

// Demo:
ImGuiProAnim::CallAnimFixedWindow("MyWindow", DataCompAnim, "CompWindow", window_flag,
	[&]() {
	    // 窗口内 ImGui 控件.
	}
);
```

### ImAnimSrcMoveColor
imgui扩展移动变色源动画控件类, 内改变绘制位置和颜色变换, 基本适用于任何控件(但是使用不当可能打乱imgui原排版).
```cpp
class ImGuiProAnim::ImAnimSrcMoveColor();
```

控件的创建和配置(Init阶段), 创建控件对象使用"CreateAnimSrcMoveColor", 配置的两组参数有 color & position, 见以下注释.
```cpp
// "copy"是当已有一个配置好的对象可以复制参数创建
ImAnimSrcMoveColor* ImGuiProAnim::CreateAnimSrcMoveColor(ImAnimSrcMoveColor* copy = nullptr);

auto demo = ImGuiProAnim::CreateAnimSrcMoveColor();

// param: Vector4T<float>
demo->config_low_color;  // 标志(false)状态颜色
demo->config_high_color; // 标志(true)状态颜色

// param:  Vector2T<float>
demo->config_low_position;  // 标志(false)状态控件绘制位置
demo->config_high_position; // 标志(true)状态控件绘制位置
```

平滑插值系数(默认为1.0, 不常用配置参数).
```cpp
demo->config_color_transspeed;
demo->config_position_transspeed;
```

控件的调用(Loop阶段), "CallAnimSrcMoveColor", 改变绘制位置-恢复源位置(具备绘制变换)通过"std::function<void(Vector4T<float>)>"包装, 并且传入颜色变换.
```cpp
// @param "input" (FrmDat.DataAnimation), "comp_name" (Key标识)
// @param "state_flag" (状态切换标志 true:high, false:low), "context" (局部绘制控件包装, 传入颜色变换)
// @param "trans_smooth" (平滑缩放)
void CallAnimSrcMoveColor(
	std::unordered_map<std::string, ImAnimationBase*>& input,
	const char*                                        comp_name,
	bool                                               state_flag,
	std::function<void(Vector4T<float>)>               context,
	float                                              trans_smooth = 1.0f
);

// Demo:
ImGuiProAnim::CallAnimSrcMoveColor(DataCompAnim, "CompSrcMove", state_flag,
	[](Vector4T<float> in_color) {
		// "in_color" 绘制颜色变换, 配合"void ImGuiPro::PushStyleColor"使用.
		// 内部控件会进行位置变换.
	}
);
```

控件的调用(Loop阶段), "CallAnimSrcMoveColorRES", 直接设置颜色 & 位置(谨慎使用), 一般用于快速复原(归位).
```cpp
// @param "input" (FrmDat.DataAnimation), "comp_name" (Key标识)
// @param "setcolor" (颜色), "setpoition" (绘制位置)
void CallAnimSrcMoveColorRES(
	std::unordered_map<std::string, ImAnimationBase*>& input,
	const char*                                        comp_name,
	Vector4T<float>                                    setcolor,
	Vector2T<float>                                    setpoition
);
```
---

简单Demo(框架内注册用户类中创建注册配置按钮动画):
```cpp
auto MyAnimButton = ImGuiProAnim::CreateAnimButton();

MyAnimButton->config_normal_size = Vector2T<float>(142.0f, 58.0f);
MyAnimButton->config_hover_size  = Vector2T<float>(150.0f, 64.0f);
MyAnimButton->config_active_size = Vector2T<float>(128.0f, 42.0f);

MyAnimButton->config_normal_color = Vector4T<float>(1.0f, 0.16f, 0.0f, 0.72f);
MyAnimButton->config_hover_color  = Vector4T<float>(0.48f, 0.0f, 1.0f, 0.85f);
MyAnimButton->config_active_color = Vector4T<float>(0.0f, 1.0f, 0.72f, 1.0f);

ImGuiProAnim::AnimCompReg(FrmDat.DataAnimation, "MyButton", MyAnimButton);
```

---

- 相对于1.1.0基本没有太大改动.

Update 20231121
