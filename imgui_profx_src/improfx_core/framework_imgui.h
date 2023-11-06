// framework_imgui. [2023_10_13] RCSZ.

#ifndef _FRAMEWORK_IMGUI_H
#define _FRAMEWORK_IMGUI_H
#include "../improfx_log/framework_log.hpp"

#include "imgui_glfw/imgui.h"
#include "imgui_glfw/imgui_impl_glfw.h"
#include "imgui_glfw/imgui_impl_opengl3.h"

#if defined(_MSV_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib,"legacy_stdio_definitons.lib")
#endif

#define SYSTEM_MODULE_LABLE_imgui std::string("[gui_imgui]: ")

// ImGui 全局控件间距(单位量).
#define IMGUI_ITEM_SPC ImGui::GetStyle().ItemSpacing.x

namespace CoreModuleImGui {

	struct GuiConfig {

		std::string     shader_version;
		std::string     fonts_filepath;
		Vector4T<float> fonts_color;
		float           fonts_size;
		float           frame_rounding, window_rounding;

		GuiConfig() :
			shader_version  ({}),
			fonts_filepath  ({}),
			fonts_color     ({}), 
			fonts_size      (16.0f),
			frame_rounding  (0.0f),
			window_rounding (0.0f)
		{}
		GuiConfig(std::string version, std::string path, Vector4T<float> color, float size, float round, float wround) :
			shader_version  (version),
			fonts_filepath  (path),
			fonts_color     (color),
			fonts_size      (size),
			frame_rounding  (round),
			window_rounding (wround)
		{}
	};

	class RenderImGui {
	protected:
		void ImGuiInit(GLFWwindow* window_object, GuiConfig cfgdata = {});
		void ImGuiFree();
		
		void RenderGuiContextA();
		void RenderGuiContextB();
	};
}

namespace ImGuiPro {
	/*
	* 全屏窗口背景(No WindowContext)
	* @param  texture_hd (纹理句柄), draw_fps = true (FPS显示)
	* @return void
	*/
	void FullScreenBackground(uint32_t texture_hd, bool draw_fps = true);
	/*
	* 全子窗口背景(Size按照当前窗口大小决定)
	* @param  texture_hd (纹理句柄), uv_cropping (uv中心裁剪比例)
	* @return void
	*/
	void FullWindowBackground(uint32_t texture_hd, ImVec2 uv_cropping = ImVec2(1.0f, 1.0f));
	/*
	* 带背景子窗口(BeginBg-ImGui::End), "FullWindowBackground"变体
	* @param  name (名称Unique), texture_hd (纹理句柄), size (固定大小), uv_cropping (uv中心裁剪比例), p_open = (bool*)0, flags = 0
	* @return void
	*/
	void BeginBg(
		const char*      name, 
		uint32_t         texture_hd, 
		ImVec2           size, 
		ImVec2           uv_cropping = ImVec2(1.0f, 1.0f), 
		bool*            p_open      = (bool*)0, 
		ImGuiWindowFlags flags       = 0
	);
	/*
	* 控件居中计算
	* @param  width (控件宽度)
	* @return float (绘制x轴起点)
	*/
	float ItemCenteredCalc(float width);
	/*
	* 居中带颜色文本
	* @param  text, color
	* @return void
	*/
	void TextCentered(const char* text, ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
	/*
	* 带文本图片按钮
	* @param  name (按钮居中文本), texture_hd (纹理句柄), size (按钮大小), id (按钮Unique编号), uv_cropping (uv中心裁剪比例)
	* @return bool (stat flag)
	*/
	bool ButtonImageText(const char* name, uint32_t texture_hd, ImVec2 size, uint32_t id, ImVec2 uv_cropping = ImVec2(1.0f, 1.0f));
	/*
	* 居中文本输入框
	* @param  name (名称Title), buffer (char*), buffer_size (length), width (输入框长度), password (查看密码标志), id (imgui unique id) 
	* @return void
	*/
	void InputTextCentered(const char* name, char* buffer, size_t buffer_size, float width, bool& password, int id);
	/*
	* 跟随鼠标悬浮窗口
	* @param  size (窗口固定大小), focus (是否常聚焦), window_ui(子窗口包装)
	* @return void
	*/
	void MouseFloatingWindow(ImVec2 size, bool focus, std::function<void()> window_ui);
	/*
	* 模拟LED状态色块
	* @param  state (true:open, false:close), h_color (高电平颜色), l_color (低电平颜色), size (rect大小)
	* @return void
	*/
	void AnalogLED(
		bool   state,
		ImVec4 h_color = ImVec4(0.0f, 1.0f, 1.0f, 1.0f),
		ImVec4 l_color = ImVec4(0.0f, 0.1f, 0.1f, 1.0f),
		ImVec2 size    = ImVec2(20.0f, 20.0f)
	);

	// ImGui::PushStyleColor 重封装 ImVec4 => Vector4T<float>
	void PushStyleColor(ImGuiCol index, Vector4T<float> color);

	// 空行 + 分割线.
	void EndEnterLine();
}

#endif