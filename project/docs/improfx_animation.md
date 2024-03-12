# ImProFx - Animation
- [x] Update.20240107 RCSZ

[返回使用文档](improfx_usage.md)
- 所在目录: src/imgui_profx_src/improfx_animation/

## 2.2.0A
> - namespace ImGuiProAnim
> - namespace ImGuiProAnimNodes
> - namespace IFC_ANIM

## ImGuiProAnim
插值动画对象储存定义.
```cpp
std::unordered_map<std::string, IFC_ANIM::ImAnimBaseStateful*> CoreModuleIMFX::FRMCORE_PACKAGE::DataCompAnim;
```

__注册:__ 注册后的控件动画对象, 如果不手动删除, 则在框架生命周期结束时自动释放.
```cpp
// @param "input" (FrmDat.DataAnimation), "key" (Key标识), "comp" (需要注册的控件对象)
// @return false:注册失败, true:注册成功
bool AnimCompReg(
    std::unordered_map<std::string, ImAnimBaseStateful*>& input, 
    const char*                                           key, 
    ImAnimBaseStateful*                                   comp
);

// Demo:
AnimCompReg(FrmDat.DataAnimation, "MyButton", Object);
```

__删除:__ 手动删除动画控件对象.
```cpp
// @param  "input" (FrmDat.DataAnimation), "key" (Key标识)
// @return false:销毁失败, true:销毁成功
bool AnimCompDelete(std::unordered_map<std::string, IFC_ANIM::ImAnimBaseStateful*>& input, const char* key);

// Demo:
AnimCompDelete(FrmDat.DataAnimation, "MyButton");
```

- 所有控件在调用"CreateAnim..."(组件对象创建)的时候都会配置默认参数.
---

### ImAnimButton
imgui扩展按钮动画控件类, 调整按钮3种状态的颜色&大小; 状态为normal(常规), hover(点击,鼠标放在控件上), active(激活,鼠标放在控件上并左键).

```cpp
class ImAnimButton
```

控件的创建和配置(Init阶段), 创建控件对象使用"CreateAnimButton", 可配置的两组参数有 color & size, 见以下注释:
```cpp
// "copy"是当已有一个配置好的对象可以复制参数创建.
ImAnimButton* CreateAnimButton(ImAnimButton* copy = nullptr);

auto demo = CreateAnimButton();

// 参数: Vector4T<float>
demo->ConfigNormalColor; // 常规状态颜色
demo->ConfigHoverColor;  // 点击状态颜色
demo->ConfigActiveColor; // 激活状态颜色

// 参数: Vector2T<float>
demo->ConfigNormalSize; // 常规状态大小
demo->ConfigHoverSize;  // 点击状态大小
demo->ConfigActiveSize; // 激活状态大小
```

平滑插值系数(默认为1.0, 不常用配置参数).
```cpp
demo->ConfigColorTransSpeed;
demo->ConfigSizeTransSpeed;
```

控件的调用(Loop阶段), "CallAnimButton"和"CallAnimButtonImage", 前者为按钮控件对应"ImGui::Button", 后者为图片(纹理)按钮对应"ImGui::ImageButton".
```cpp
// @param  "button_name" (按钮名称), "input" (FrmDat.DataAnimation)
// @param  "comp_name" (Key标识), "trans_smooth" (步进缩放)
// @return 按钮状态标志
bool CallAnimButton(
	const char*                                           button_name,
	std::unordered_map<std::string, ImAnimBaseStateful*>& input,
	const char*                                           comp_name,
	const float&                                          trans_smooth = 1.0f
);

// @param (button_name ~ comp_name 和 trans_smooth 跟以上注释功能相同)
// @param  "texture_hd" (OpenGL纹理句柄)
// @return 按钮状态标志
bool CallAnimButtonImage(
	const char*                                           button_name,
	std::unordered_map<std::string, ImAnimBaseStateful*>& input,
	const char*                                           comp_name,
	uint32_t                                              texture_hd,
	const float&                                          trans_smooth = 1.0f
);
```
---

### ImAnimFixedWindow
imgui扩展固定窗口动画控件类, 调整窗口2种状态的 颜色 & 大小; 子窗口状态为close(关闭), open(打开).

```cpp
class ImAnimFixedWindow
```

控件的创建和配置(Init阶段), 创建控件对象使用"CreateAnimFixedWindow", 配置的两组参数有 color & size, 见以下注释.
```cpp
// "copy"是当已有一个配置好的对象可以复制参数创建
ImAnimFixedWindow* CreateAnimFixedWindow(ImAnimFixedWindow* copy = nullptr);

auto demo = CreateAnimFixedWindow();

// param: Vector4T<float>
demo->ConfigOpenColor;  // 子窗口打开状态背景颜色
demo->ConfigCloseColor; // 子窗口关闭状态背景颜色(虽然不可见主要与打开状态产生过渡)

// param: Vector2T<float>
demo->ConfigOpenSize;  // 子窗口打开状态固定大小
demo->ConfigCloseSize; // 子窗口关闭状态大小(虽然不可见主要与打开状态产生过渡)
```

平滑插值系数(默认为1.0, 不常用配置参数).
```cpp
demo->ConfigColorTransSpeed;
demo->ConfigSizeTransSpeed;
```

控件的调用(Loop阶段), "CallAnimFixedWindow", 对应"ImGui::Begin - ImGui::End", 通过"std::function<void()>"包装.
```cpp
// @param "window_name" (窗口名称), "input" (FrmDat.DataAnimation)
// @param "comp_name" (Key标识), "open_flag" (窗口状态, 参考ImGui::Begin的bool* p_open标志)
// @param "window_ui" (窗口函数对象), "flags" (窗口标志, 与ImGui::Begin的ImGuiWindowFlags Flags等价)
// @param "trans_smooth" (步进缩放)
void CallAnimFixedWindow(
	const char*                                           window_name, 
	std::unordered_map<std::string, ImAnimBaseStateful*>& input,
	const char*                                           comp_name,
	bool&                                                 open_flag,
	std::function<void()>                                 window_ui,
	ImGuiWindowFlagsAm                                    flags = 0,
	const float&                                          trans_smooth = 1.0f
);

// Demo:
CallAnimFixedWindow("MyWindow", DataCompAnim, "CompWindow", window_flag,
	[&]() {
	    // 窗口内 ImGui 控件.
	}
);
```
---

### ImAnimSourceComp
imgui扩展源动画控件类, 内改变绘制位置,大小和颜色变换, 基本适用于任何控件(但是使用不当可能打乱imgui原排版).

```cpp
class ImAnimSourceComp;
```

控件的创建和配置(Init阶段), 创建控件对象使用"CreateAnimSourceComp", 配置的三组参数有 color & size & position, 见以下注释.
```cpp
// "copy"是当已有一个配置好的对象可以复制参数创建
ImAnimSourceComp* CreateAnimSourceComp(ImAnimSourceComp* copy = nullptr);

auto demo = CreateAnimSourceComp();

// param: Vector4T<float>
demo->ConfigLowColor;  // 标志(false)状态颜色
demo->ConfigHighColor; // 标志(true)状态颜色

// param: Vector2T<float>
demo->ConfigLowSize;  // 标志(false)状态绘制大小
demo->ConfigHighSize; // 标志(true)状态绘制大小

// param: Vector2T<float>
demo->ConfigLowPosition;  // 标志(false)状态控件绘制位置
demo->ConfigHighPosition; // 标志(true)状态控件绘制位置
```

平滑插值系数(默认为1.0, 不常用配置参数).
```cpp
demo->ConfigColorTransSpeed;
demo->ConfigSizeTransSpeed;
demo->ConfigPositionTransSpeed;
```

控件的调用(Loop阶段), "CallAnimSourceComp", 改变绘制位置-恢复源位置(具备绘制变换)通过"std::function<void(Vector4T<float>)>"包装, 并且传入颜色变换.
```cpp
// @param "input" (FrmDat.DataAnimation), "comp_name" (Key标识)
// @param "state_flag" (状态切换标志 true:high, false:low), "context" (绘制函数对象, 参数传入: 大小&颜色)
// @param "trans_smooth" (平滑缩放)
void CallAnimSourceComp(
	std::unordered_map<std::string, ImAnimBaseStateful*>& input,
	const char*                                           comp_name,
	const bool&                                           state_flag,
	std::function<void(Vector2T<float>, Vector4T<float>)> context,
	const float&                                          trans_smooth = 1.0f
);

// Demo:
CallAnimSourceComp(DataCompAnim, "CompSrcMove", state_flag,
	[&](Vector2T<float> in_size, Vector4T<float> in_color) {
		// "in_size" 绘制大小变换.
		// "in_color" 绘制颜色变换, 配合"void ImGuiPro::PushStyleColor"使用.
		// 内部控件会进行位置变换.
	}
);
```

控件的调用(Loop阶段), "CallAnimSourceCompRES", 直接设置颜色 & 位置(谨慎使用), 一般用于快速复原(归位).
```cpp
// @param "input" (FrmDat.DataAnimation), "comp_name" (Key标识)
// @param "setcolor" (颜色), "setsize" (绘制大小), "setpoition" (绘制位置)
void CallAnimSourceCompRES(
	std::unordered_map<std::string, ImAnimBaseStateful*>& input,
	const char*                                           comp_name,
	Vector4T<float>                                       setcolor,
	Vector2T<float>                                       setsize,
	Vector2T<float>                                       setpoition
);
```
---

简单Demo(框架内注册用户类中创建注册配置按钮动画):
```cpp
auto MyAnimButton = CreateAnimButton();

MyAnimButton->ConfigNormalSize = Vector2T<float>(142.0f, 58.0f);
MyAnimButton->ConfigHoverSize  = Vector2T<float>(150.0f, 64.0f);
MyAnimButton->ConfigActiveSize = Vector2T<float>(128.0f, 42.0f);

MyAnimButton->ConfigNormalColor = Vector4T<float>(1.0f, 0.16f, 0.0f, 0.72f);
MyAnimButton->ConfigHoverColor  = Vector4T<float>(0.48f, 0.0f, 1.0f, 0.85f);
MyAnimButton->ConfigActiveColor = Vector4T<float>(0.0f, 1.0f, 0.72f, 1.0f);

AnimCompReg(FrmDat.DataAnimation, "MyButton", MyAnimButton);
```

---

## ImGuiProAnimNodes
插值节点动画对象内部定义.
```cpp
std::unordered_map<std::string, IFC_ANIM::ImAnimBaseDynamic*> CoreModuleIMFX::FRMCORE_PACKAGE::DataAnimationNodes;
```

__注册:__ 注册后的控件动画对象, 如果不手动删除, 则在框架生命周期结束时自动释放.
```cpp
// @param "input" (FrmDat.DataAnimationNodes), "key" (Key标识), "comp" (需要注册的控件对象)
// @return false:注册失败, true:注册成功
bool AnimNodesCompReg(
	std::unordered_map<std::string, IFC_ANIM::ImAnimBaseDynamic*>& input,
	const char*                                                    key,
	IFC_ANIM::ImAnimBaseDynamic*                                   comp
);

// Demo:
AnimCompReg(FrmDat.DataAnimationNodes, "MyNodes", Object);
```

__删除:__ 手动删除动画控件对象.
```cpp
// @param  "input" (FrmDat.DataAnimationNodes), "key" (Key标识)
// @return false:销毁失败, true:销毁成功
bool AnimNodesCompDelete(std::unordered_map<std::string, IFC_ANIM::ImAnimBaseDynamic*>& input, const char* key);

// Demo:
AnimCompDelete(FrmDat.DataAnimation, "MyNodes");
```

__创建:__ 与以上动画插值不同, 节点动画统一使用一个函数注册控件对象.
```cpp
enum AnimCompType {
	NodesHashmap = 1 << 1, // 注册Hashmap型节点动画对象(参见以下)
	NodesVector  = 1 << 2  // 注册Vector型节点动画对象(参见以下)
};

// @param "type" (以上创建模式选择), "copy" (复制配置的控件对象), 
// @param "speed" (插值速度缩放, x:位置, y:大小, z:颜色)
// "copy"是当已有一个配置好的对象可以复制参数创建(与以上一样)
IFC_ANIM::ImAnimBaseDynamic* CreateAnimNodes(
	AnimCompType                 type,
	IFC_ANIM::ImAnimBaseDynamic* copy  = nullptr,
	const Vector3T<float>&       speed = Vector3T<float>(1.0f, 1.0f, 1.0f)
);
```

- 所有节点无初始化配置参数, 并且注册时必须保证至少有一个节点, 否则会抛出警告.
---

### ImAnimNodesHashmap
imgui扩展节点动画(哈希容器), 使用Key-Value搜索节点切换状态, 位置变换&大小变换&颜色变换.

```cpp
class ImAnimNodesHashmap
```

配置(Init阶段)添加&删除节点, Key必须为唯一.
```cpp
// 节点配置编辑函数:
// "state" (节点状态), "pos" (节点变换位置), "size" (节点变换大小), "color" (节点变换颜色)
IFC_ANIM::ImAnimDynamicNode ConfigAnimNode(
	const bool&            state,
	const Vector2T<float>& pos,
	const Vector2T<float>& size,
	const Vector4T<float>& color = Vector4T<float>(1.0f, 1.0f, 1.0f, 1.0f)
);

// @param "key" (节点Key标识), "value" (节点参数, 可以使用以上函数编辑)
void ConfigNodeAdd(const char* key, IFC_ANIM::ImAnimDynamicNode value);
// @param "key" (Key标识)
void ConfigNodeDelete(const char* key);
```

查询节点(Init阶段), 用于拷贝配置以后局部修改.
```cpp
// @param "key" (节点Key标识)
IFC_ANIM::ImAnimDynamicNode* ConfigNodeFind(const char* key);
```

调用节点动画(Loop阶段).
```cpp
#define SYSDEF_FUNC_NODES_PARAM void(const Vector2T<float>&, const Vector4T<float>&, const bool&)

// @param  "nodes_key" (节点Key标识), "input" (FrmDat.DataAnimationNodes), 
// @param  "context" (局部绘制控件函数对象, 内会进行位置变换, 传入大小&颜色变换&状态参数), "trans_smooth" (平滑缩放)
// @return true:调用成功, false:调用失败
bool CallAnimNodesHashmap(
	const char*                                                    nodes_key,
	std::unordered_map<std::string, IFC_ANIM::ImAnimBaseDynamic*>& input,
	const char*                                                    comp_name,
	std::function<SYSDEF_FUNC_NODES_PARAM>                         context,
	const float&                                                   trans_smooth = 1.0f
);
```
---

### ImAnimNodesVector
imgui扩展节点动画(Vector容器), 使用Count定位节点切换状态(适用于依次连续变换), 位置变换&大小变换&颜色变换.

```cpp
class ImAnimNodesVector
```

配置(Init阶段)添加&删除节点, Key可重复.
```cpp
// 节点配置编辑函数:
// "state" (节点状态), "pos" (节点变换位置), "size" (节点变换大小), "color" (节点变换颜色)
IFC_ANIM::ImAnimDynamicNode ConfigAnimNode(
	const bool&            state,
	const Vector2T<float>& pos,
	const Vector2T<float>& size,
	const Vector4T<float>& color = Vector4T<float>(1.0f, 1.0f, 1.0f, 1.0f)
);

// @param "key" (Key标识), "value" (节点参数, 可以使用以上函数编辑)
void ConfigNodeAdd(const char* key, IFC_ANIM::ImAnimDynamicNode value);
// @param "key" (Key标识, 会删除所有相同的Key)
void ConfigNodeDelete(const char* key);
```

查询节点(Init阶段), 用于拷贝配置以后局部修改.
```cpp
// @param "key" (Count的str形式, 例:"1")
IFC_ANIM::ImAnimDynamicNode* ConfigNodeFind(const char* key);
```

调用节点动画(Loop阶段).
```cpp
#define SYSDEF_FUNC_NODES_PARAM void(const Vector2T<float>&, const Vector4T<float>&, const bool&)

// @param  "nodes_count" (节点编号), "input" (FrmDat.DataAnimationNodes), 
// @param  "context" (局部绘制控件函数对象, 内会进行位置变换, 传入大小&颜色变换&状态参数), "trans_smooth" (平滑缩放)
// @return true:调用成功, false:调用失败
bool CallAnimNodesHashmap(
	const size_t&                                                  nodes_count,
	std::unordered_map<std::string, IFC_ANIM::ImAnimBaseDynamic*>& input,
	const char*                                                    comp_name,
	std::function<SYSDEF_FUNC_NODES_PARAM>                         context,
	const float&                                                   trans_smooth = 1.0f
);
```
---

简单Demo(框架内注册用户类中创建注册配置节点动画):
```cpp
// 创建节点动画:
auto MyNodes = ImGuiProAnimNodes::CreateAnimNodes(ImGuiProAnimNodes::NodesVector, nullptr, Vector3T<float>(0.58f, 1.0f, 0.32f));

// 配置节点:
MyNodes->ConfigNodeAdd("N1", ImGuiProAnimNodes::ConfigAnimNode(true, Vector2T<float>(...), Vector2T<float>(...), Vector4T<float>(...)));
MyNodes->ConfigNodeAdd("N2", ImGuiProAnimNodes::ConfigAnimNode(true, Vector2T<float>(...), Vector2T<float>(...), Vector4T<float>(...)));
MyNodes->ConfigNodeAdd("N3", ImGuiProAnimNodes::ConfigAnimNode(true, Vector2T<float>(...), Vector2T<float>(...), Vector4T<float>(...)));

// 注册节点动画:
ImGuiProAnimNodes::AnimNodesCompReg(FrmDat.DataAnimationNodes, "MyNodesObj", MyNodes);

// 调用时可以直接使用"NODESFUNC"宏:
#define NODESFUNC [&](const Vector2T<float>& size, const Vector4T<float>& color, const bool& state)
// 调用节点动画:
ImGuiProAnimNodes::CallAnimNodesVector(NodesCount, FrmDat.DataAnimationNodes, "MyNodesObj", 
	NODESFUNC{
		// 在此调用 size & color & state 参数.
		// 会自动进行设置绘制位置(ImGui)
	}
);
```
---

- Stateful控件添加内置ImGui帧控件ID避免框架名称冲突.
- 依然建议使用自制控件, 动画模块局限性太大.

>- 2023_11_02 version 1.0.0 Alpha
>- 2023_11_05 version 1.1.0 Alpha
>- 2023_11_21 version 2.0.0 Test1
>- 2023_12_02 version 2.0.0 Test2
>- 2023_12_08 version 2.0.0 Alpha
>- 2023_12_16 version 2.1.0 Alpha
>- 2024_01_07 version 2.2.0 Alpha

|动画控件Class结构| 版本 |
|----|----|
|ImAnimBaseStateful -> ImAnimButton | 1.0.0A |
|ImAnimBaseStateful -> ImAnimFixedWindow | 1.0.0A |
|ImAnimBaseStateful -> ImAnimSrcMoveColor | 1.1.0A |
|ImAnimBaseDynamic -> NULL | 2.0.0T2 |
|ImAnimBaseDynamic -> ImAnimNodesHashmap | 2.0.0A |
|ImAnimBaseDynamic -> ImAnimNodesVector | 2.0.0A |

```END```