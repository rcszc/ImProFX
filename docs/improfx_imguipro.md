# ImProFx - ImGuiPro
- [x] Update.20231216 RCSZ

[返回使用文档](improfx_usage.md)
- 所在目录: src/imgui_profx_src/improfx_core/

> 2023_11_02 version 1.0.0 Alpha
> 2023_11_05 version 1.1.0 Alpha
> 2023_11_21 version 2.0.0 Test-1
> 2023_12_02 version 2.0.0 Test-2
> 2023_12_08 version 2.0.0 Alpha
> 2023_12_16 version 2.1.0 Alpha

## 2.1.0A
> 纯函数基于ImGui的扩展控件, 与动画类无关(但是也有简易插值).

### ImGuiPro
> namespace ImGuiPro 扩展控件

函数中的 __UV中心裁剪__ 参数 "ImVec2 uv_cropping": 比如vec2.x为0.5f, UV的U向为0.0f-1.0f, 那么裁剪为U向中点+-二分之一, UV的U向就变成0.25f-0.75f, V向以此类推; 参见以下实现:
```cpp
// UV min & UV max
ImVec2(0.5f - uv_cropping.x / 2.0f, 0.5f - uv_cropping.y / 2.0f);
ImVec2(0.5f + uv_cropping.x / 2.0f, 0.5f + uv_cropping.y / 2.0f);
```

覆盖型背景, 全屏背景 & 全窗口背景:
```cpp
// @param "texture_hd" (OpenGL纹理句柄), "draw_fps" (是否显示FPS[左上角])
void FullScreenBackground(uint32_t texture_hd, bool draw_fps = true);

// @param "texture_hd" (OpenGL纹理句柄), "uv_cropping" (uv中心裁剪)
// 此函数在 ImGui::Begin - ImGui::End 上下文内调用
void FullWindowBackground(uint32_t texture_hd);
```

带背景子窗口("全窗口背景"的封装), 配合"ImGui::End()"使用.
```cpp
// @param "name" (窗口名称), "texture_hd" (OpenGL纹理句柄), "size" (窗口固定大小)
// @param "uv_cropping" (uv中心裁剪), "p_open" (参考ImGui::Begin)
// @param "flags" (参考ImGui::Begin)
void BeginBg(
	const char*      name, 
	uint32_t         texture_hd, 
	ImVec2           size, 
	ImVec2           uv_cropping = ImVec2(1.0f, 1.0f), 
	bool*            p_open      = (bool*)0, 
	ImGuiWindowFlags flags       = 0
);
```

ImGui控件居中计算(按照ImGui窗口宽度).
```cpp
// @param  "width" (需要居中的控件宽度)
// @return 控件起始绘制x轴
float ItemCenteredCalc(float width);

// 一般配合设置绘制位置使用
ImGui::SetCursorPosX(ImGuiPro::ItemCalcCentered(256.0f));
```

带颜色标题文本 P1 P2 P3 (使用子窗口与窗口字体缩放).
```cpp
// @param "text" (文本), "child_winx" (子窗口宽度), "color" (文本颜色), "border" (是否显示边框[方便调试])
void TextTitleP1(const char* text, const float& child_winx, const ImVec4& color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f), const bool& border = false);
void TextTitleP2(const char* text, const float& child_winx, const ImVec4& color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f), const bool& border = false);
void TextTitleP3(const char* text, const float& child_winx, const ImVec4& color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f), const bool& border = false);
```

带颜色居中文本非标题(按照ImGui窗口宽度).
```cpp
// @param "text" (文本), "color" (文本颜色)
void TextCentered(const char* text, ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
```

圆角图片显示(配合ImGui::BeginChild - ImGui::EndChild).
```cpp
// @param "texture_hd" (OpenGL纹理句柄), "position" (图片绘制位置), "size" (图片绘制大小)
// @param "round" (圆角大小), "uv_cropping" (uv中心裁剪)
void RoundImage(
	const uint32_t& texture_hd, 
	const ImVec2&   position, 
	const ImVec2&   size, 
	const float&    round       = 5.8f, 
	const ImVec2&   uv_cropping = ImVec2(1.0f, 1.0f)
);
```

图片按钮带文本(文本为按钮中居中).
```cpp
// @param  "text" (文本), "texture_hd" (OpenGL纹理句柄), "size" (按钮绘制大小)
// @param  "id" (Unique编号), "uv_cropping" (uv中心裁剪)
// @return 按钮标志
bool ButtonImageText(
	const char*     name, 
	const uint32_t& texture_hd, 
	const ImVec2&   size, 
	const uint32_t& id, 
	const ImVec2&   uv_cropping = ImVec2(1.0f, 1.0f)
);
```

跟随鼠标悬浮窗口 悬浮值 & 悬浮图片.
```cpp
// @param "value" / "texture_hd" (float值 / OpenGL纹理句柄), "limit" / "size" (xy上下限 / 图片绘制大小)
// @param "color" 值进度条颜色
void FloatingWindowValue(const float& value, const ImVec2& limit, const ImVec4& color = ImVec4(1.0f, 1.0f, 1.0f, 0.72f));
void FloatingWindowImage(const uint32_t& texture_hd, const ImVec2& size = ImVec2(96.0f, 96.0f));
```

__输入框__ 数据结构体:
```cpp
struct inputchar {
	char*  input_buffer_pointer; // 输入内容 char_buffer 指针
	size_t input_buffer_size;    // char_buffer 长度
}
```

带颜色边框提升文本输入框 颜色边框 & 输入警告.
```cpp
// @param "name" (输入框名称), "data" (输入数据), "width" (输入框宽度), "color" (输入框颜色)
void InputTextWireframe(const char* name, const inputchar& data, const float& width, const ImVec4& color);

// @param "name" (输入框名称), "data" (输入数据), "width" (输入框宽度), "state" (输入内容状态 true:h_color, false:l_color)
// @param "h_color" (高亮[警告]颜色), "l_color" (正常颜色)
void InputTextStatusPrompt(
	const char*       name,
	const inputchar&  data,
	const float&      width,
	const bool&       state,
	const ImVec4&     h_color = ImVec4(0.0f, 1.0f, 1.0f, 0.72f),
	const ImVec4&     l_color = ImVec4(1.0f, 0.0f, 0.0f, 0.72f)
);
```

居中文本输入框 普通 & 带密码隐藏.
```cpp
// @param "text" (文本), "data" (输入数据), "width" (输入框宽度)
// @param "password" (true:隐藏信息)
void InputCenteredText(const char* name, inputchar data, const float& width);
void InputCenteredTextPsw(const char* name, inputchar data, const float& width, bool password = false);
```

多行文本显示(多行文本输入-只读模式) 显示文本 & 动画显示文本.
```cpp
// @param "name" (文本框名称), "text_str" (文本内容), "size" (文本框大小)
void ViewTextMultiline(const char* name, std::string& text_str, const ImVec2& size = ImVec2(0.0f, 0.0f));

// @param "name" (文本框名称), "text_str" (文本内容)
// @param "count" (文本显示字数[只需要传入静态float初始化为0.0f]), "dis" (显示状态, true:动画开始, false:动画复原)
// @param "size" (文本框大小), "speed" (动画速度缩放)
void ViewTextMultilineAnim(
	const char*   name, 
	std::string&  text_str, 
	float&        count,
	const bool&   dis,
	const ImVec2& size  = ImVec2(0.0f, 0.0f),
	const float&  speed = 1.0f
);
```

倒三角选择框(原ImGui选择框为勾,修改为到三角)
```cpp
// 参考: ImGui::Checkbox 函数的用法
void CheckboxTriangle(const char* name, bool* p_flag);
```

判断窗口滚动页Y轴是否到达区间.
```cpp
// @param  "limit" (滚动区间, x:min, y:max)
// @return 触发标志(true:到达)
bool DetePosScrollY(ImVec2 limit);
```

滑条(float)平滑插值.
```cpp
// @param "name" (滑条名称), "value" (滑条buffer, x:滑条值, y:滑条插值), "limit" (滑条值范围)
// @param "speed" (插值速度缩放), "format" (参考ImGui::SliderFloat), "flags" (参考ImGui::SliderFloat)
void InterSliderFloat(
	const char*      name,
	Vector2T<float>& f_buffer,
	ImVec2           limit,
	float            trans_speed = 1.0f,
	const char*      format      = "%.3f", 
	ImGuiSliderFlags flags       = 0
);
```

滚动页面鼠标平滑(窗口必须使用"ImGuiWindowFlags_NoScrollWithMouse"标志, 因为此函数会接管鼠标滚轮操作).
```cpp
// @param "pos" (滚动页面Y轴位置, x:页面位置, Y:页面插值位置), "speed" (滚动页面速度缩放)
// @param "wheel" (是否启用滚轮滚动), "focus" (是否启用窗口焦点)
// 最后两项参数为: 如果手动接管页面滚动, 即设置为false
void SmoothYSlide(Vector2T<float>& ypos, const float& speed = 1.0f, const bool& wheel = true, const bool& focus = true);
```

模拟LED指示灯(ImGui::Button变形).
```cpp
// @param "state" (true:open, false:close), "size" (rect大小), "h_color" (高电平颜色), "l_color" (低电平颜色)
void LndicatorLED(
	const bool&   state,
	const ImVec2& size    = ImVec2(20.0f, 20.0f),
	const ImVec4& h_color = ImVec4(0.0f, 1.0f, 1.0f, 1.0f),
	const ImVec4& l_color = ImVec4(0.0f, 0.2f, 0.2f, 0.8f)
);
```

ImGuiPush颜色样式函数重封装.
```cpp
// ImGui::PushStyleColor 重封装 ImVec4 => Vector4T<float>
void ImGuiPro::PushStyleColor(ImGuiCol index, Vector4T<float> color);
```

 空行 + 分割线 + 空行 (一般用于布局结尾).
```cpp
void ImGuiPro::EndEnterLine();
```

### ImGuiProRZ
> namespace ImGuiProRZ 扩展模板控件

横向滚动图片页面(A模板).
```cpp
void TemplatePageLSA(
	const char*                  name,                 // 子窗口名称
	ImVec2                       winsize,              // 子窗口大小(决定页面图片显示大小)
	const std::vector<uint32_t>& pageimgs,             // 滚动图片OpenGL纹理句柄
	const Vector2T<bool>&        playflag,             // x:向前一张, y:向后一张
	Vector2T<float>&             xpos,                 // 位置x_buffer(x:位置, y:插值位置)
	const bool&                  border      = true,   // 显示边框
	const float&                 transparent = 1.0f,   // 页面透明度
	ImGuiWindowFlags             winflags    = 0       // 子窗口标志(参考ImGui::BeginChild)
);
```

数据排行页面窗口(A模板, warn:实时拷贝排序可能开销较大).
```cpp
void TemplatePageFDSA(
	const char*                                   name, // 子窗口名称
	const std::unordered_map<std::string, float>& data, // 待排序显示数据
	const bool&                                   border = false,                            // 显示边框
	const ImVec4&                                 color  = ImVec4(0.42f, 0.0f, 0.88f, 0.78f) // 排行榜(进度条)颜色
);
```

可缩放APP布局子窗口模板(16:9, 12:16).
```cpp
// 16:9 A模板
void TemplatePageApp169A(
	const char*           unique_name,         // 子窗口名称
	std::function<void()> child_win_0,         // 布局区域0函数对象
	std::function<void()> child_win_1,         // 布局区域1函数对象
	std::function<void()> child_win_2,         // 布局区域2函数对象
	std::function<void()> child_win_3,         // 布局区域3函数对象
	const float&          global_scale = 1.0f, // 全局等比缩放
	const bool&           border       = true  // 显示边框
);
// 16:9 B模板
void TemplatePageApp169B( 
	const char*           unique_name, // 参考以上
	std::function<void()> child_win_0,
	std::function<void()> child_win_1,
	std::function<void()> child_win_2,
	std::function<void()> child_win_3,
	const float&          global_scale = 1.0f,
	const bool&           border       = true
);
// 12:16 A模板
void TemplatePageApp1216A(
	const char*           unique_name, // 参考以上
	std::function<void()> child_win_0,
	std::function<void()> child_win_1,
	std::function<void()> child_win_2,
	const float&          global_scale = 1.0f,
	const bool&           border       = true
);
```

框架ImGui全局调试(Debug)窗口(warn:内有static变量).
```cpp
// @param "p_open" (窗口标志, 参考ImGui::Begin)
void RZsystemDebugWindow(bool* p_open = (bool*)0);
```
---

- 基于ImGui纯函数扩展控件, 与"inprofx_animation"动画系统无关, 但是其中也有简单插值动画

```END```