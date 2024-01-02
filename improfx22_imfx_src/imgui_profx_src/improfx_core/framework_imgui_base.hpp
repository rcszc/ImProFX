// framework_imgui_base. ImGui控件封装.

#ifndef _FRAMEWORK_IMGUI_BASE_HPP
#define _FRAMEWORK_IMGUI_BASE_HPP
#include "../improfx_log/framework_log.hpp"

#include "imgui_glfw/imgui.h"
#include "imgui_glfw/imgui_impl_glfw.h"
#include "imgui_glfw/imgui_impl_opengl3.h"

#if defined(_MSV_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib,"legacy_stdio_definitons.lib")
#endif

#define SYSMD_LB_CONTROL "[fxgui_control]: "

enum ImTypeInputInt {
	// imgui_control function type:'ImGui::InputInt'
	InputInt1 = 1 << 1,
	InputInt2 = 1 << 2,
	InputInt3 = 1 << 3,
	InputInt4 = 1 << 4
};

enum ImTypeInputFloat {
	// imgui_control function type:'ImGui::InputFloat'
	InputFloat1 = 1 << 5,
	InputFloat2 = 1 << 6,
	InputFloat3 = 1 << 7,
	InputFloat4 = 1 << 8
};

enum ImTypeSliderInt {
	// imgui_control function type:'ImGui::SliderInt'
	SliderInt1 = 1 << 1,
	SliderInt2 = 1 << 2,
	SliderInt3 = 1 << 3,
	SliderInt4 = 1 << 4
};

enum ImTypeSliderFloat {
	// imgui_control function type:'ImGui::SliderFloat'
	SliderFloat1 = 1 << 5,
	SliderFloat2 = 1 << 6,
	SliderFloat3 = 1 << 7,
	SliderFloat4 = 1 << 8
};

enum ImTypeColorEdit {
	// imgui_control function type:'ImGui::ColorEdit'
	ColorEdit3 = 1 << 1,
	ColorEdit4 = 1 << 2
};

enum ImTypeColorPicker {
	// imgui_control function type:'ImGui::ColorPicker'
	ColorPicker3 = 1 << 3,
	ColorPicker4 = 1 << 4
};

namespace SYSIMGUI_COUPLING {
	// global atomic variable: imgui unique_id.
	extern std::atomic<uint32_t> ImGuiControlUniqueIDcount;

	// framework imgui_control class.
	// update: 20231218 RCSZ
	// file_struct: framework_imgui_control.hpp => framework_imgui.h
	class FrameworkImGuiControlBase {
	protected:
		std::atomic<uint32_t>* ImGuiControlUniqueID = nullptr;
		ImGuiIO* ImGuiDataIO = {};

		void ControlPushIDcount() {
			ImGui::PushID(ImGuiControlUniqueID);
			++ImGuiControlUniqueID;
		}
	public:
		FrameworkImGuiControlBase(std::atomic<uint32_t>* AtomicCount) {
			ImGuiControlUniqueID = AtomicCount;
			ImGuiDataIO = &ImGui::GetIO();
			LOGCONS::PushLogger(LOG_TRACE, SYSMD_LB_CONTROL, "create imgui_control base object.");
		}
		~FrameworkImGuiControlBase() {
			LOGCONS::PushLogger(LOG_TRACE, SYSMD_LB_CONTROL, "free imgui_control base object.");
		}
		ImGuiIO* GetImGuiIO() { return ImGuiDataIO; }

		// ######################################## imgui general_controls ########################################
		// ImGui常规控件.

		ImVec2 CalcTextSize(const char* text, const char* text_end = (const char*)0, bool hide_after_double_hash = false, float wrap_width = -1.0f);

		void Text(const char* fmt_text, ...);
		void TextColored(const ImVec4& color, const char* fmt_text, ...);
		bool Button(const char* name, const ImVec2& size = ImVec2(0.0f, 0.0f));
		bool ColorButton(const char* name, const ImVec4& color, const ImGuiColorEditFlags& flags = 0, const ImVec2& size = ImVec2(0.0f, 0.0f));
		bool Checkbox(const char* name, bool* flagptr);
		bool RadioButton(const char* name, bool active);
		bool InputText(const char* name, char* buffer, size_t size, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
		void ProgressBar(float fraction, const ImVec2& size_arg = ImVec2(-1, 0), const char* overlay = NULL);
		bool InputTextMultiline(
			const char*            name,
			char*                  buf,
			size_t                 buf_size,
			const ImVec2&          size      = ImVec2(0.0f,0.0f),
			ImGuiInputTextFlags    flags     = 0,
			ImGuiInputTextCallback callback  = (ImGuiInputTextCallback)0,
			void*                  user_data = (void*)0
		);
		void PlotLines(
			const char*  name,
			const float* values,
			int          values_count,
			int          values_offset = 0,
			const char*  overlay_text  = (const char*)0,
			float        scale_min     = FLT_MIN,
			float        scale_max     = FLT_MAX,
			ImVec2       graph_size    = ImVec2(0.0f, 0.0f),
			int          stride        = 4
		);

		void SameLine() { ImGui::SameLine(); }
		void Indent(float indent_w = 0.0f) { ImGui::Indent(indent_w); }
		void Unindent(float indent_w = 0.0f) { ImGui::Unindent(indent_w); }

		void Image(
			ImTextureID   user_texture_id,
			const ImVec2& size,
			const ImVec2& uv0        = ImVec2(0.0f, 0.0f),
			const ImVec2& uv1        = ImVec2(1.0f, 1.0f),
			const ImVec4& tint_col   = ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
			const ImVec4& border_col = ImVec4(0.0f, 0.0f, 0.0f, 0.0f)
		);

		bool ImageButton(
			const char*   name,
			ImTextureID   user_texture_id,
			const ImVec2& size,
			const ImVec2& uv0        = ImVec2(0.0f, 0.0f),
			const ImVec2& uv1        = ImVec2(1.0f, 1.0f),
			const ImVec4& tint_col   = ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
			const ImVec4& border_col = ImVec4(0.0f, 0.0f, 0.0f, 0.0f)
		);

		bool InputInt(ImTypeInputInt type, const char* name, int* valueptr, int step = 0.0f, int step_fast = 0.0f, ImGuiInputTextFlags flags = 0);
		bool InputFloat(ImTypeInputFloat type, const char* name, float* valueptr, float step = 0.0f, float step_fast = 0.0f, const char* format = "%.3f", ImGuiInputTextFlags flags = 0);

		bool SliderInt(ImTypeSliderInt type, const char* name, int* valueptr, int min, int max, const char* display_format = "%.3f", ImGuiSliderFlags flags = 0);
		bool SliderFloat(ImTypeSliderFloat type, const char* name, float* valueptr, float min, float max, const char* display_format = "%.3f", ImGuiSliderFlags flags = 0);

		bool ColorEdit(ImTypeColorEdit type, const char* name, float* color, ImGuiColorEditFlags flags = 0);
		bool ColorPicker(ImTypeColorPicker type, const char* name, float* color, ImGuiColorEditFlags flags = 0);

		void DrawTable(const char* name, const std::vector<std::string>& headers, const std::vector<std::vector<std::string>>& data, ImGuiTableFlags flags = 0);

		void ImGuiWindow(const char* name, std::function<void()> window_function, bool* p_open = (bool*)0, ImGuiWindowFlags flags = 0);
		void ImGuiWindowChild(
			const char*           name,
			std::function<void()> window_function,
			const ImVec2&         size   = ImVec2(0.0f, 0.0f),
			bool                  border = false,
			ImGuiWindowFlags      flags  = 0
		);
		void ImGuiWindowTootip(std::function<void()> window_function);

		void PushStyleVar(ImGuiStyleVar index, float value);
		void PushStyleColor(ImGuiCol index, const ImVec4& color);

		void PopStyleVar(uint32_t count = 1);
		void PopStyleColor(uint32_t count = 1);

		// ******************** imgui get_state ********************

		ImVec2 GetCursorPos() { return ImGui::GetCursorPos(); }
		ImVec2 GetWindowPos() { return ImGui::GetWindowPos(); }
		ImVec2 GetWindowSize() { return ImGui::GetWindowSize(); }
		ImVec4 GetStyleColorVec4(ImGuiCol index) { return ImGui::GetStyleColorVec4(index); }

		bool IsWindowFocused(ImGuiFocusedFlags flags = 0) { return ImGui::IsWindowFocused(flags); }
		bool IsWindowCollapsed() { return ImGui::IsWindowCollapsed(); }
		bool IsWindowAppearing() { return ImGui::IsWindowAppearing(); }

		bool IsItemHovered(ImGuiHoveredFlags flags = 0) { return ImGui::IsItemHovered(flags); }
		bool IsItemActive() { return ImGui::IsItemActive(); }
		bool IsItemActivated() { return ImGui::IsItemActivated(); }
		bool IsItemClicked(ImGuiMouseButton flags = 0) { return ImGui::IsItemClicked(flags); }

		bool IsMouseDown(ImGuiMouseButton button) { return ImGui::IsMouseDown(button); }
		bool IsMouseClicked(ImGuiMouseButton button, bool repeat = false) { return ImGui::IsMouseClicked(button, repeat); }

		// ******************** imgui set_state ********************

		void SetCursorPos(const ImVec2& position) { ImGui::SetCursorPos(position); }
		void SetCursorPosX(float xpos) { ImGui::SetCursorPosX(xpos); }
		void SetCursorPosY(float ypos) { ImGui::SetCursorPosY(ypos); }

		void SetNextWindowPos(const ImVec2& position) { ImGui::SetNextWindowPos(position); }
		void SetNextWindowSize(const ImVec2& size) { ImGui::SetNextWindowSize(size); }
		void SetNextItemWidth(float width) { ImGui::SetNextItemWidth(width); }

		// ######################################## imgui tool_controls ########################################
		// ImGui工具控件使用'ImGui::GetWindowDrawList'等.

		void ToolDrawRectangle(const ImVec2& position, const ImVec2& size, const ImVec4& color, const float& linewidth = 3.2f);
		void ToolDrawRectangleFill(const ImVec2& position, const ImVec2& size, const ImVec4& color);
		bool ToolHoverRectangle(const ImVec2& position, const ImVec2& size);
		void ToolDrawTriangleFill(const ImVec2& position, const ImVec2& offset1, const ImVec2& offset2, const ImVec4& color);
		void ToolDrawImageUVscale(const uint32_t& TextureHandle, const ImVec2& position, const ImVec2& size, const float& scale);
	};
}

#endif