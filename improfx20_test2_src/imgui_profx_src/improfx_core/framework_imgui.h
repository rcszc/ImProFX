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

#define SYSMD_LB_IMGUI "[gui_imgui]: "

// ImGui 全局控件间距(单位量).
#define IMGUI_ITEM_SPC ImGui::GetStyle().ItemSpacing.x

namespace basic_drawlist_comp {

	void draw_rectangle(const ImVec2& position, const ImVec2& size, const ImVec4& color, const float& linewidth = 3.2f);
	void draw_rectangle_fill(const ImVec2& position, const ImVec2& size, const ImVec4& color);
	bool hover_rectangle(const ImVec2& position, const ImVec2& size);

	void draw_triangle_fill(const ImVec2& position, const ImVec2& offset1, const ImVec2& offset2, const ImVec4& color);

	void draw_image_uvscale(const uint32_t& texture_hd, const ImVec2& position, const ImVec2& size, const float& scale);

	void draw_text_scale(const char* text, const float& scale, const ImVec4& color);
}

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

// "ImGuiPro" 基于imgui的扩展控件.
namespace ImGuiPro {

	struct inputchar {
		char*  input_buffer_pointer;
		size_t input_buffer_size;

		constexpr inputchar() : input_buffer_pointer(nullptr), input_buffer_size(NULL) {}
		constexpr inputchar(char* pointer, size_t size_byte) :
			input_buffer_pointer (pointer),
			input_buffer_size    (size_byte)
		{}
	};
	
	void FullScreenBackground(const uint32_t& texture_hd, const bool& draw_fps = true);
	void FullWindowBackground(const uint32_t& texture_hd, const ImVec2& uv_cropping = ImVec2(1.0f, 1.0f), const float& yoffset = 0.0f);
	
	void BeginBackground(
		const char*      name, 
		const uint32_t&  texture_hd, 
		const ImVec2&    size, 
		const ImVec2&    uv_cropping = ImVec2(1.0f, 1.0f), 
		bool*            p_open      = (bool*)0, 
		ImGuiWindowFlags flags       = 0
	);
	
	float ItemCalcCentered(const float& width);
	
	void TextTitleP1(const char* text, const float& child_winx, const ImVec4& color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f), const bool& border = false);
	void TextTitleP2(const char* text, const float& child_winx, const ImVec4& color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f), const bool& border = false);
	void TextTitleP3(const char* text, const float& child_winx, const ImVec4& color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f), const bool& border = false);

	void TextCentered(const char* text, const ImVec4& color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
	
	void RoundImage(
		const uint32_t& texture_hd, 
		const ImVec2&   position, 
		const ImVec2&   size, 
		const float&    round       = 5.8f, 
		const ImVec2&   uv_cropping = ImVec2(1.0f, 1.0f)
	);

	bool ButtonImageText(
		const char*     name, 
		const uint32_t& texture_hd, 
		const ImVec2&   size, 
		const uint32_t& id, 
		const ImVec2&   uv_cropping = ImVec2(1.0f, 1.0f)
	);
	
	void FloatingWindowValue(const float& value, const ImVec2& limit, const ImVec4& color = ImVec4(1.0f, 1.0f, 1.0f, 0.72f));
	void FloatingWindowImage(const uint32_t& texture_hd, const ImVec2& size = ImVec2(96.0f, 96.0f));

	void InputTextWireframe(const char* name, const inputchar& data, const float& width, const ImVec4& color);
	void InputTextStatusPrompt(
		const char*      name,
		const inputchar& data,
		const float&     width,
		const bool&      state,
		const ImVec4&    h_color = ImVec4(0.0f, 1.0f, 1.0f, 0.72f),
		const ImVec4&    l_color = ImVec4(1.0f, 0.0f, 0.0f, 0.72f)
	);

	void InputCenteredText(const char* name, inputchar data, const float& width);
	void InputCenteredTextPsw(const char* name, inputchar data, const float& width, bool password = false);

	void ViewTextMultiline(const char* name, std::string& text_str, const ImVec2& size = ImVec2(0.0f, 0.0f));
	// [STC-ANIM]
	void ViewTextMultilineAnim(
		const char*   name, 
		std::string&  text_str, 
		float&        count,
		const bool&   dis,
		const ImVec2& size = ImVec2(0.0f, 0.0f),
		const float&  speed = 1.0f
	);

	void CheckboxTriangle(const char* name, bool* p_flag);
	
	bool DetePosScrollY(ImVec2 limit);
	// [STC-ANIM]
	void InterSliderFloat(
		const char*      name,
		Vector2T<float>& f_buffer,
		ImVec2           limit,
		float            trans_speed = 1.0f,
		const char*      format      = "%.3f", 
		ImGuiSliderFlags flags       = 0
	);
	// [STC-ANIM]
	void SmoothYSlide(Vector2T<float>& ypos, const float& speed = 1.0f);
	
	void LndicatorLED(
		const bool&   state,
		const ImVec2& size    = ImVec2(20.0f, 20.0f),
		const ImVec4& h_color = ImVec4(0.0f, 1.0f, 1.0f, 1.0f),
		const ImVec4& l_color = ImVec4(0.0f, 0.2f, 0.2f, 0.8f)
	);

	// ImGui::PushStyleColor 重封装 ImVec4 => Vector4T<float>
	void PushStyleColor(const ImGuiCol& index, const Vector4T<float>& color);

	// Draw: 空行 + 分割线 + 空行.
	void EndEnterLine();
}

// "ImGuiProRZ" 更高级的扩展模板,会用到 "ImGuiPro"控件.
namespace ImGuiProRZ {

	// lateral sliding A[child_window]. [STC-ANIM]
	void TemplatePageLSA(
		const char*                  name, 
		ImVec2                       winsize, 
		const std::vector<uint32_t>& pageimgs, 
		const Vector2T<bool>&        playflag,
		Vector2T<float>&             xpos,
		const bool&                  border      = true,
		const float&                 transparent = 1.0f,
		ImGuiWindowFlags             winflags    = 0
	);

	// float data sorting A[child_window].
	void TemplatePageFDSA(
		const char*                                   name,
		const std::unordered_map<std::string, float>& data,
		const bool&                                   border = false,
		const ImVec4&                                 color  = ImVec4(0.42f, 0.0f, 0.88f, 0.78f)
	);

	constexpr Vector2T<float> WIN_PRESET_SIZE169  = Vector2T<float>(896.0f, 504.0f);
	constexpr Vector2T<float> WIN_PRESET_SIZE1216 = Vector2T<float>(576.0f, 768.0f);

	// app[16:9] gui A[child_window].
	void TemplatePageApp169A(
		const char*           unique_name,
		std::function<void()> child_win_0,
		std::function<void()> child_win_1,
		std::function<void()> child_win_2,
		std::function<void()> child_win_3,
		const float&          global_scale = 1.0f,
		const bool&           border       = true
	);

	// app[16:9] gui B[child_window].
	void TemplatePageApp169B(
		const char*           unique_name,
		std::function<void()> child_win_0,
		std::function<void()> child_win_1,
		std::function<void()> child_win_2,
		std::function<void()> child_win_3,
		const float&          global_scale = 1.0f,
		const bool&           border       = true
	);

	// app[12:16] gui A[child_window].
	void TemplatePageApp1216A(
		const char*           unique_name,
		std::function<void()> child_win_0,
		std::function<void()> child_win_1,
		std::function<void()> child_win_2,
		const float&          global_scale = 1.0f,
		const bool&           border       = true
	);

	// system debug window: global unique(static).
	void RZsystemDebugWindow(bool* p_open = (bool*)0);
}
	
#endif