// framework_imgui_control. ImGui控件封装.

#ifndef _FRAMEWORK_IMGUI_CONTROL_HPP
#define _FRAMEWORK_IMGUI_CONTROL_HPP
#include "../improfx_log/framework_log.hpp"

#include "imgui_glfw/imgui.h"
#include "imgui_glfw/imgui_impl_glfw.h"
#include "imgui_glfw/imgui_impl_opengl3.h"

#if defined(_MSV_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib,"legacy_stdio_definitons.lib")
#endif

#define SYSMD_LB_CONTROL "[gui_control]: "

enum ImTypeInputInt 
{
	InputInt1 = 1 << 1,
	InputInt2 = 1 << 2,
	InputInt3 = 1 << 3,
	InputInt4 = 1 << 4
};
enum ImTypeInputFloat 
{
	InputFloat1 = 1 << 1,
	InputFloat2 = 1 << 2,
	InputFloat3 = 1 << 3,
	InputFloat4 = 1 << 4
};

enum ImTypeSliderInt 
{
	SliderInt1 = 1 << 1,
	SliderInt2 = 1 << 2,
	SliderInt3 = 1 << 3,
	SliderInt4 = 1 << 4
};
enum ImTypeSliderFloat 
{
	SliderFloat1 = 1 << 1,
	SliderFloat2 = 1 << 2,
	SliderFloat3 = 1 << 3,
	SliderFloat4 = 1 << 4
};

enum ImTypeColorEdit
{
	ColorEdit3 = 1 << 1,
	ColorEdit4 = 1 << 2
};
enum ImTypeColorPicker
{
	ColorPicker3 = 1 << 1,
	ColorPicker4 = 1 << 2
};

// framework imgui_control class.
// update: 20231215 RCSZ
class FrameworkImGuiControl {
protected:
	ImGuiIO* ImGuiDataIO = {};
	int ImGuiControlUniqueID = -1;

	void ControlPushIDcount() {
		ImGui::PushID(ImGuiControlUniqueID);
		--ImGuiControlUniqueID;
	}

	void ControlUpdateFrame() {
		for (int i = -1; i >= ImGuiControlUniqueID; --i)
			ImGui::PopID();
		ImGuiControlUniqueID = NULL;
	}

public:
	FrameworkImGuiControl() {
		ImGuiDataIO = &ImGui::GetIO();
	}
	ImGuiIO* GetImGuiIO() { return ImGuiDataIO; }

	void Text(const char* fmt_text, ...) {
		va_list ParamArgs;
		va_start(ParamArgs, fmt_text);
		ImGui::TextV(fmt_text, ParamArgs);
		va_end(ParamArgs);
	}

	bool Button(const char* name, const ImVec2& size = ImVec2(0.0f, 0.0f)) {
		ControlPushIDcount();
		return ImGui::Button(name, size);
	}

	bool ColorButton(const char* name, const ImVec4& color, const ImGuiColorEditFlags& flags = 0, const ImVec2& size = ImVec2(0.0f, 0.0f)) {
		ControlPushIDcount();
		return ImGui::ColorButton(name, color, flags, size);
	}

	bool Checkbox(const char* name, bool* flagptr) {
		ControlPushIDcount();
		return ImGui::Checkbox(name, flagptr);
	}

	bool RadioButton(const char* name, bool active) {
		ControlPushIDcount();
		return ImGui::RadioButton(name, active);
	}

	bool InputText(const char* name, char* buffer, size_t size, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL) {
		ControlPushIDcount();
		return ImGui::InputText(name, buffer, size, flags, callback, user_data);
	}

	void ProgressBar(float fraction, const ImVec2& size_arg = ImVec2(-1, 0), const char* overlay = NULL) {
		ImGui::ProgressBar(fraction, size_arg, overlay);
	}

	void Image(
		ImTextureID   user_texture_id,
		const ImVec2& size,
		const ImVec2& uv0        = ImVec2(0.0f, 0.0f),
		const ImVec2& uv1        = ImVec2(1.0f, 1.0f),
		const ImVec4& tint_col   = ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
		const ImVec4& border_col = ImVec4(0.0f, 0.0f, 0.0f, 0.0f)
	) {
		ImGui::Image(user_texture_id, size, uv0, uv1, tint_col, border_col);
	}

	bool ImageButton(
		const char*   name,
		ImTextureID   user_texture_id,
		const ImVec2& size,
		const ImVec2& uv0        = ImVec2(0.0f, 0.0f),
		const ImVec2& uv1        = ImVec2(1.0f, 1.0f),
		const ImVec4& tint_col   = ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
		const ImVec4& border_col = ImVec4(0.0f, 0.0f, 0.0f, 0.0f)
	) {
		ControlPushIDcount();
		return ImGui::ImageButton(name, user_texture_id, size, uv0, uv1, tint_col, border_col);
	}

	bool InputInt(ImTypeInputInt type, const char* name, int* valueptr, int step = 0.0f, int step_fast = 0.0f, ImGuiInputTextFlags flags = 0) {
		ControlPushIDcount();
		switch (type) {
		case(InputInt1): { return ImGui::InputInt(name, valueptr, step, step_fast, flags); }
		case(InputInt2): { return ImGui::InputInt2(name, valueptr, flags); }
		case(InputInt3): { return ImGui::InputInt3(name, valueptr, flags); }
		case(InputInt4): { return ImGui::InputInt4(name, valueptr, flags); }
		default: return false;
		}
	}

	bool InputFloat(ImTypeInputFloat type, const char* name, float* valueptr, float step = 0.0f, float step_fast = 0.0f, const char* format = "%.3f", ImGuiInputTextFlags flags = 0) {
		ControlPushIDcount();
		switch (type) {
		case(InputFloat1): { return ImGui::InputFloat(name, valueptr, step, step_fast, format, flags); }
		case(InputFloat2): { return ImGui::InputFloat2(name, valueptr, format, flags); }
		case(InputFloat3): { return ImGui::InputFloat3(name, valueptr, format, flags); }
		case(InputFloat4): { return ImGui::InputFloat4(name, valueptr, format, flags); }
		default: return false;
		}
	}

	bool SliderInt(ImTypeSliderInt type, const char* name, int* valueptr, int min, int max, const char* display_format = "%.3f", ImGuiSliderFlags flags = 0) {
		ControlPushIDcount();
		switch (type) {
		case(SliderInt1): { return ImGui::SliderInt(name, valueptr, min, max, display_format, flags);  }
		case(SliderInt2): { return ImGui::SliderInt2(name, valueptr, min, max, display_format, flags); }
		case(SliderInt3): { return ImGui::SliderInt3(name, valueptr, min, max, display_format, flags); }
		case(SliderInt4): { return ImGui::SliderInt4(name, valueptr, min, max, display_format, flags); }
		default: return false;
		}
	}

	bool SliderFloat(ImTypeSliderFloat type, const char* name, float* valueptr, float min, float max, const char* display_format = "%.3f", ImGuiSliderFlags flags = 0) {
		ControlPushIDcount();
		switch (type) {
		case(SliderFloat1): { return ImGui::SliderFloat(name, valueptr, min, max, display_format, flags);  }
		case(SliderFloat2): { return ImGui::SliderFloat2(name, valueptr, min, max, display_format, flags); }
		case(SliderFloat3): { return ImGui::SliderFloat3(name, valueptr, min, max, display_format, flags); }
		case(SliderFloat4): { return ImGui::SliderFloat4(name, valueptr, min, max, display_format, flags); }
		default: return false;
		}
	}

	bool ColorEdit(ImTypeColorEdit type, const char* name, float* color, ImGuiColorEditFlags flags = 0) {
		ControlPushIDcount();
		switch (type) {
		case(ColorEdit3): { return ImGui::ColorEdit3(name, color, flags); }
		case(ColorEdit4): { return ImGui::ColorEdit4(name, color, flags); }
		default: return false;
		}
	}

	bool ColorPicker(ImTypeColorPicker type, const char* name, float* color, ImGuiColorEditFlags flags = 0) {
		ControlPushIDcount();
		switch (type) {
		case(ColorPicker3): { return ImGui::ColorPicker3(name, color, flags); }
		case(ColorPicker4): { return ImGui::ColorPicker4(name, color, flags); }
		default: return false;
		}
	}

	void DrawTable(const char* name, const std::vector<std::string>& headers, const std::vector<std::vector<std::string>>& data, ImGuiTableFlags flags = 0) {
		// "data"必须为矩阵类型数组.
		if (headers.empty() || data.empty() || data[0].empty()) {
			LOGCONS::PushLogger(LOG_WARN, SYSMD_LB_CONTROL, "draw_table data: headers == 0 || x == 0 || y == 0.");
			return;
		}
		if (headers.size() != data[0].size()) {
			LOGCONS::PushLogger(LOG_WARN, SYSMD_LB_CONTROL, "draw_table data: headers != cols.");
			return;
		}

		int NumberRows = (int)data.size();
		int NumberCols = (int)headers.size();

		ControlPushIDcount();
		if (ImGui::BeginTable(name, NumberCols, flags)) {
			// draw table headers.
			for (int col = 0; col < NumberCols; ++col)
				ImGui::TableSetupColumn(headers[col].c_str());
			ImGui::TableHeadersRow();

			for (int row = 0; row < NumberRows; ++row) {
				for (int col = 0; col < NumberCols; ++col) {

					ImGui::TableNextColumn();
					ImGui::Text("%s", data[row][col].c_str());
				}
			}
			ImGui::EndTable();
		}
	}

	void ImGuiWindow(const char* name, std::function<void()> window_function, bool* p_open = (bool*)0, ImGuiWindowFlags flags = 0) {
		ControlPushIDcount();
		ImGui::Begin(name, p_open, flags);
		window_function();
		ImGui::End();
	}

	void ImGuiWindowChild(
		const char*           name, 
		std::function<void()> window_function, 
		const ImVec2&         size   = ImVec2(0.0f,0.0f), 
		bool                  border = false, 
		bool*                 p_open = (bool*)0, 
		ImGuiWindowFlags      flags  = 0
	) {
		ControlPushIDcount();
		ImGui::BeginChild(name, size, border, flags);
		window_function();
		ImGui::End();
	}

	void PushStyleVar(ImGuiStyleVar index, const float& value) {
		ImGui::PushStyleVar(index, value);
	}
	void PopStyleVar(uint32_t count = 1) {
		ImGui::PopStyleVar((int)count);
	}

	void PushStyleColor(ImGuiCol index, const ImVec4& color) {
		ImGui::PushStyleColor(index, color);
	}
	void PopStyleColor(uint32_t count = 1) {
		ImGui::PopStyleColor((int)count);
	}
};

#endif