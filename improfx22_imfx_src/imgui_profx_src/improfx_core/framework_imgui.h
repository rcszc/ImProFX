// framework_imgui. [2023_10_13] RCSZ.

#ifndef _FRAMEWORK_IMGUI_H
#define _FRAMEWORK_IMGUI_H
#include "framework_imgui_base.hpp"

#define SYSMD_LB_IMGUI "[fxgui_imgui]: "

// ImGui 全局控件间距(单位量).
#define IMGUI_ITEM_SPC ImGui::GetStyle().ItemSpacing.x

namespace CoreModuleImGui {

	struct GuiConfig {
		std::string     ShaderVersionStr;
		std::string     FontsFilepath;
		Vector4T<float> FontsGlobalColor;
		float           FontsGlobalSize;
		float           FrameRounding, WindowRounding;

		GuiConfig() :
			ShaderVersionStr ({}),
			FontsFilepath    ({}),
			FontsGlobalColor ({}), 
			FontsGlobalSize  (16.0f),
			FrameRounding    (0.0f),
			WindowRounding   (0.0f)
		{}
		GuiConfig(std::string version, std::string path, Vector4T<float> color, float size, float round, float wround) :
			ShaderVersionStr (version),
			FontsFilepath    (path),
			FontsGlobalColor (color),
			FontsGlobalSize  (size),
			FrameRounding    (round),
			WindowRounding   (wround)
		{}
	};

	class RenderImGui {
	protected:
		std::function<void(uint32_t)> ImPopUniqueIDFunc = [](uint32_t) {};

		void ImGuiInit(GLFWwindow* window_object, GuiConfig cfgdata = {});
		void ImGuiFree();
		
		void RenderGuiContextA();
		void RenderGuiContextB();
	};
}

// 'ImGuiPro' 基于 'ImGuiBase' 的扩展控件.
namespace ImGuiPro {

	struct InputCharPtr {
		char*  InputBufferPointer;
		size_t InputBufferSize;

		constexpr InputCharPtr() : InputBufferPointer(nullptr), InputBufferSize(NULL) {}
		constexpr InputCharPtr(char* pointer, size_t size_byte) :
			InputBufferPointer (pointer),
			InputBufferSize    (size_byte)
		{}
	};
	
	void FullScreenImWindowBg(const uint32_t& texture_hd, const bool& draw_fps = true);
	
	class FrameworkImGuiControl :public SYSIMGUI_COUPLING::FrameworkImGuiControlBase {
	public:
		FrameworkImGuiControl(std::atomic<uint32_t>* AC) : FrameworkImGuiControlBase(AC) {}

		void PRO_FullWindowBackground(uint32_t texture_hd, const ImVec2& uv_cropping = ImVec2(1.0f, 1.0f), float yoffset = 0.0f);
		void PRO_ImGuiWindowBackground(
			const char*           name,
			const uint32_t&       texture_hd,
			const ImVec2&         size,
			std::function<void()> window_function,
			const ImVec2&         uv_cropping = ImVec2(1.0f, 1.0f),
			bool*                 p_open      = (bool*)0,
			ImGuiWindowFlags      flags       = 0
		);

		float PRO_ItemCalcCentered(float width);

		void PRO_TextTitleP1(const char* text, float child_winx, const ImVec4& color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f), const bool& border = false);
		void PRO_TextTitleP2(const char* text, float child_winx, const ImVec4& color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f), const bool& border = false);
		void PRO_TextTitleP3(const char* text, float child_winx, const ImVec4& color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f), const bool& border = false);

		void PRO_TextCentered(const char* text, const ImVec4& color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

		void PRO_RoundImage(
			const uint32_t& texture_hd,
			const ImVec2&   position,
			const ImVec2&   size,
			float           round       = 5.8f,
			const ImVec2&   uv_cropping = ImVec2(1.0f, 1.0f)
		);

		bool PRO_ButtonImageText(
			const char*     name,
			const uint32_t& texture_hd,
			const ImVec2&   size,
			const ImVec2&   uv_cropping = ImVec2(1.0f, 1.0f)
		);

		void PRO_FloatingWindowValue(float value, const ImVec2& limit, const ImVec4& color = ImVec4(1.0f, 1.0f, 1.0f, 0.72f));
		void PRO_FloatingWindowImage(uint32_t texture_hd, const ImVec2& size = ImVec2(96.0f, 96.0f));

		void PRO_InputTextWireframe(const char* name, const InputCharPtr& data, float width, const ImVec4& color);
		void PRO_InputTextStatusPrompt(
			const char*      name,
			const InputCharPtr& data,
			float            width,
			bool             state,
			const ImVec4&    high_color = ImVec4(0.0f, 1.0f, 1.0f, 0.72f),
			const ImVec4&    low_color  = ImVec4(1.0f, 0.0f, 0.0f, 0.72f)
		);

		void PRO_InputCenteredText(const char* name, const InputCharPtr& data, float width);
		void PRO_InputCenteredTextPsw(const char* name, const InputCharPtr& data, float width, bool password = false);

		void PRO_ViewTextMultiline(const char* name, std::string& text_str, const ImVec2& size = ImVec2(0.0f, 0.0f));
		void PRO_ViewTextMultilineAnim( // [ANIMATION]
			const char*        name,
			const std::string& text_str,
			float&             count,
			bool               dis,
			const ImVec2&      size  = ImVec2(0.0f, 0.0f),
			float              speed = 1.0f
		);

		void PRO_CheckboxTriangle(const char* name, bool* p_flag);

		bool PRO_DetePosScrollY(ImVec2 limit);

		void PRO_InterSliderFloat( // [ANIMATION]
			const char*      name,
			Vector2T<float>& f_buffer,
			ImVec2           limit,
			float            trans_speed = 1.0f,
			const char*      format      = "%.3f",
			ImGuiSliderFlags flags       = 0
		);
		// [ANIMATION] (聚焦当前窗口时生效)
		void PRO_SmoothYSlide(Vector2T<float>& ypos, float speed = 1.0f, bool wheel = true, bool focus = true);

		void PRO_LndicatorLED(
			bool          state,
			const ImVec2& size       = ImVec2(20.0f, 20.0f),
			const ImVec4& high_color = ImVec4(0.0f, 1.0f, 1.0f, 1.0f),
			const ImVec4& low_color  = ImVec4(0.0f, 0.2f, 0.2f, 0.8f)
		);

		// 空行 + 分割线 + 空行.
		void PRO_EndEnterLine();
	};
}

// 'FrameworkImGuiControlBase' => 'FrameworkImGuiControl' => 'FrameworkGUI'
namespace ImProFXgui {

#define DEBUG_FPS_SAMPLE_LEN   128
	struct DebugWindowBuffer {
		bool   LogWindowOpenFlag  = false;
		int    LogWindowSmpLines = 72;
		float  FpsSampleBuffer[DEBUG_FPS_SAMPLE_LEN] = {};
	};

	constexpr Vector2T<float> WIN_PRESET_SIZE169 = Vector2T<float>(896.0f, 504.0f);
	constexpr Vector2T<float> WIN_PRESET_SIZE1216 = Vector2T<float>(576.0f, 768.0f);

	class FrameworkGUI :public ImGuiPro::FrameworkImGuiControl {
	public:
		FrameworkGUI(std::atomic<uint32_t>* IAC) : FrameworkImGuiControl(IAC) {};

		// lateral sliding A[child_window]. [ANIMATION]
		void PRORZ_TemplatePageLSA(
			const char*                  name,
			ImVec2                       winsize,
			const std::vector<uint32_t>& pageimgs,
			const Vector2T<bool>&        playflag,
			Vector2T<float>&             xpos,
			bool                         border      = true,
			float                        transparent = 1.0f,
			ImGuiWindowFlags             winflags    = 0
		);

		// float data sorting A[child_window].
		void PRORZ_TemplatePageSortA(
			const char*                                   name,
			const std::unordered_map<std::string, float>& data,
			bool                                          border = false,
			const ImVec4&                                 color  = ImVec4(0.42f, 0.0f, 0.88f, 0.78f)
		);

		// layout[16:9] gui A[child_window].
		void PRORZ_TemplatePageLayout169A(
			const char*           name,
			std::function<void()> child_win_0,
			std::function<void()> child_win_1,
			std::function<void()> child_win_2,
			std::function<void()> child_win_3,
			float                 global_scale = 1.0f,
			bool                  border       = true
		);

		// layout[16:9] gui B[child_window].
		void PRORZ_TemplatePageLayout169B(
			const char*           name,
			std::function<void()> child_win_0,
			std::function<void()> child_win_1,
			std::function<void()> child_win_2,
			std::function<void()> child_win_3,
			float                 global_scale = 1.0f,
			bool                  border       = true
		);

		// layout[12:16] gui A[child_window].
		void PRORZ_TemplatePageLayout1216A(
			const char*           name,
			std::function<void()> child_win_0,
			std::function<void()> child_win_1,
			std::function<void()> child_win_2,
			float                 global_scale = 1.0f,
			bool                  border       = true
		);

		// system debug window.
		void RZsystemDebugWindow(DebugWindowBuffer& buffer, bool* p_open = (bool*)0);
	};
}
	
#endif