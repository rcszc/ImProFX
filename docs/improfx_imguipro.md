# ImProFx - ImGuiPro
RCSZ

- 所在目录: src\imgui_profx_src\improfx_core\

> 2023_11_02 version 1.0.0 ahpla

## 1.0.0A
> 纯函数的ImGui控件普通扩展, 与动画无关

namespace ImGuiPro

全屏窗口背景(覆盖整个GLFW窗口).
```cpp
// @param "texture_hd" OpenGL纹理句柄, "draw_fps" 是否显示FPS(左上角)
void ImGuiPro::FullScreenBackground(uint32_t texture_hd, bool draw_fps = true);
```

全子窗口背景(覆盖整个ImGui子窗口,ImGui::Begin - ImGui::End).
```cpp
// @param "texture_hd" OpenGL纹理句柄
// 此函数在 begin - end 上下文内调用.
void ImGuiPro::FullWindowBackground(uint32_t texture_hd);
```

带背景子窗口("全子窗口背景"的封装), 配合"ImGui::End()"使用. "uv_cropping" UV中心裁剪举例: 比如vec2.x为0.5f, UV的U向为0.0f-1.0f, 那么裁剪为U向中点+-二分之一, UV的U向就变成0.25f-0.75f, V向以此类推.
```cpp
// @param "name" 窗口名称, "texture_hd" OpenGL纹理句柄, "size" 窗口固定大小
// @param "uv_cropping" uv中心裁剪, "p_open" 参考ImGui::Begin
// @param "flags" 参考ImGui::Begin
void ImGuiPro::BeginBg(
    const char* name, uint32_t texture_hd, ImVec2 size, 
    ImVec2 uv_cropping = ImVec2(1.0f, 1.0f), bool* p_open = (bool*)0,
    ImGuiWindowFlags flags = 0
);
```

ImGui控件居中计算(按照ImGui窗口宽度).
```cpp
// @param  "width" 控件宽度
// @return 控件起始绘制x轴
float ImGuiPro::ItemCenteredCalc(float width);
```

带颜色居中文本(按照ImGui窗口宽度).
```cpp
// @param "text" 文本, "color" 文本颜色
void ImGuiPro::TextCentered(const char* text, ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
```

带图片文本按钮(文本为按钮中居中).
```cpp
// @param "text" 文本, "texture_hd" OpenGL纹理句柄, "size" 按钮大小
// @param "id" Unique编号, "uv_cropping" uv中心裁剪(参考以上)
bool ImGuiPro::ButtonImageText(
    const char* name, uint32_t texture_hd, ImVec2 size, 
    uint32_t id, ImVec2 uv_cropping = ImVec2(1.0f, 1.0f)
);
```

居中文本输入框(带密码隐藏).
```cpp
// @param "text" 文本, "buffer" 文本buffer, "buffer_size" length
// @param "width" 输入框宽度, "password" true:隐藏信息, "id" Unique编号
void ImGuiPro::InputTextCentered(
    const char* name, char* buffer, size_t buffer_size, 
    float width, bool& password, int id
);
```

跟随鼠标悬浮窗口(固定大小一般用于注解).
```cpp
// @param "size" 悬浮子窗大小, "focus" 常聚焦悬浮窗口, "window_ui" 子窗口内控件包装.
void ImGuiPro::MouseFloatingWindow(ImVec2 size, bool focus, std::function<void()> window_ui);
```

模拟LED色块(ImGui::Button变形).
```cpp
// @param "state" (true:open, false:close), "h_color" (高电平颜色), "l_color" (低电平颜色), "size" (rect大小)
void ImGuiPro::AnalogLED(
	bool   state,
	ImVec4 h_color = ImVec4(0.0f, 1.0f, 1.0f, 1.0f),
	ImVec4 l_color = ImVec4(0.0f, 0.1f, 0.1f, 1.0f),
	ImVec2 size    = ImVec2(20.0f, 20.0f)
);
```

空行+分割线(一般用于竖向布局结尾).
```cpp
void ImGuiPro::EndEnterLine();
```

---

- Update 20231102
