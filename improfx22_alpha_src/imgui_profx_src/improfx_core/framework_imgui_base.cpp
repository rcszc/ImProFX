// framework_imgui_base.

#include "framework_imgui_base.hpp"

using namespace std;
using namespace LOGCONS;

namespace SYSIMGUI_COUPLING {
	std::atomic<uint32_t> ImGuiControlUniqueIDcount(0);

	// **************************************** imgui general_controls ****************************************

	ImVec2 FrameworkImGuiControlBase::CalcTextSize(const char* text, const char* text_end, bool hide_after_double_hash, float wrap_width) {
		ImGui::CalcTextSize(text, text_end, hide_after_double_hash, wrap_width);
	}

	void FrameworkImGuiControlBase::Text(const char* fmt_text, ...) {
		va_list ParamArgs;
		va_start(ParamArgs, fmt_text);
		ImGui::TextV(fmt_text, ParamArgs);
		va_end(ParamArgs);
	}

	void FrameworkImGuiControlBase::TextColored(const ImVec4& color, const char* fmt_text, ...) {
		va_list ParamArgs;
		va_start(ParamArgs, fmt_text);
		ImGui::TextColoredV(color, fmt_text, ParamArgs);
		va_end(ParamArgs);
	}

	bool FrameworkImGuiControlBase::Button(const char* name, const ImVec2& size) {
		ControlPushIDcount();
		bool ButtonFlag = ImGui::Button(name, size);
		ImGui::PopID();
		return ButtonFlag;
	}

	bool FrameworkImGuiControlBase::ColorButton(const char* name, const ImVec4& color, const ImGuiColorEditFlags& flags, const ImVec2& size) {
		ControlPushIDcount();
		bool ColorButtonFlag = ImGui::ColorButton(name, color, flags, size);
		ImGui::PopID();
		return ColorButtonFlag;
	}

	bool FrameworkImGuiControlBase::Checkbox(const char* name, bool* flagptr) {
		ControlPushIDcount();
		bool CheckboxFlag = ImGui::Checkbox(name, flagptr);
		ImGui::PopID();
		return CheckboxFlag;
	}

	bool FrameworkImGuiControlBase::RadioButton(const char* name, bool active) {
		ControlPushIDcount();
		bool RadioButtonFlag = ImGui::RadioButton(name, active);
		ImGui::PopID();
		return RadioButtonFlag;
	}

	bool FrameworkImGuiControlBase::InputText(const char* name, char* buffer, size_t size, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data) {
		ControlPushIDcount();
		bool InputTextFlag = ImGui::InputText(name, buffer, size, flags, callback, user_data);
		ImGui::PopID();
		return InputTextFlag;
	}

	void FrameworkImGuiControlBase::ProgressBar(float fraction, const ImVec2& size_arg, const char* overlay) {
		ImGui::ProgressBar(fraction, size_arg, overlay);
	}

	bool FrameworkImGuiControlBase::InputTextMultiline(
		const char*            name, 
		char*                  buf, 
		size_t                 buf_size, 
		const ImVec2&          size, 
		ImGuiInputTextFlags    flags, 
		ImGuiInputTextCallback callback, 
		void*                  user_data
	) {
		ControlPushIDcount();
		bool InputTextMultilineFlag = ImGui::InputTextMultiline(name, buf, buf_size, size, flags, callback, user_data);
		ImGui::PopID();
		return InputTextMultilineFlag;
	}

	void FrameworkImGuiControlBase::PlotLines(
		const char*  name, 
		const float* values, 
		int          values_count, 
		int          values_offset, 
		const char*  overlay_text,
		float        scale_min, 
		float        scale_max, 
		ImVec2       graph_size, 
		int          stride
	) {
		ControlPushIDcount();
		ImGui::PlotLines(name, values, values_count, values_offset, overlay_text, scale_min, scale_max, graph_size, stride);
		ImGui::PopID();
	}

	void FrameworkImGuiControlBase::Image(
		ImTextureID   user_texture_id,
		const ImVec2& size,
		const ImVec2& uv0,
		const ImVec2& uv1,
		const ImVec4& tint_col,
		const ImVec4& border_col
	) {
		ImGui::Image(user_texture_id, size, uv0, uv1, tint_col, border_col);
	}

	bool FrameworkImGuiControlBase::ImageButton(
		const char*   name,
		ImTextureID   user_texture_id,
		const ImVec2& size,
		const ImVec2& uv0,
		const ImVec2& uv1,
		const ImVec4& tint_col,
		const ImVec4& border_col
	) {
		ControlPushIDcount();
		bool ImageButtonFlag = ImGui::ImageButton(name, user_texture_id, size, uv0, uv1, tint_col, border_col);
		ImGui::PopID();
		return ImageButtonFlag;
	}

	bool FrameworkImGuiControlBase::InputInt(ImTypeInputInt type, const char* name, int* valueptr, int step, int step_fast, ImGuiInputTextFlags flags) {
		ControlPushIDcount();
		bool InputIntFlag = false;
		switch (type) {
		case(InputInt1): { InputIntFlag = ImGui::InputInt(name, valueptr, step, step_fast, flags); break; }
		case(InputInt2): { InputIntFlag = ImGui::InputInt2(name, valueptr, flags); break; }
		case(InputInt3): { InputIntFlag = ImGui::InputInt3(name, valueptr, flags); break; }
		case(InputInt4): { InputIntFlag = ImGui::InputInt4(name, valueptr, flags); break; }
		}
		ImGui::PopID();
		return InputIntFlag;
	}

	bool FrameworkImGuiControlBase::InputFloat(ImTypeInputFloat type, const char* name, float* valueptr, float step, float step_fast, const char* format, ImGuiInputTextFlags flags) {
		ControlPushIDcount();
		bool InputFloatFlag = false;
		switch (type) {
		case(InputFloat1): { InputFloatFlag = ImGui::InputFloat(name, valueptr, step, step_fast, format, flags); break; }
		case(InputFloat2): { InputFloatFlag = ImGui::InputFloat2(name, valueptr, format, flags); break; }
		case(InputFloat3): { InputFloatFlag = ImGui::InputFloat3(name, valueptr, format, flags); break; }
		case(InputFloat4): { InputFloatFlag = ImGui::InputFloat4(name, valueptr, format, flags); break; }
		}
		ImGui::PopID();
		return InputFloatFlag;
	}

	bool FrameworkImGuiControlBase::SliderInt(ImTypeSliderInt type, const char* name, int* valueptr, int min, int max, const char* display_format, ImGuiSliderFlags flags) {
		ControlPushIDcount();
		bool SliderIntFlag = false;
		switch (type) {
		case(SliderInt1): { SliderIntFlag = ImGui::SliderInt(name, valueptr, min, max, display_format, flags);  break; }
		case(SliderInt2): { SliderIntFlag = ImGui::SliderInt2(name, valueptr, min, max, display_format, flags); break; }
		case(SliderInt3): { SliderIntFlag = ImGui::SliderInt3(name, valueptr, min, max, display_format, flags); break; }
		case(SliderInt4): { SliderIntFlag = ImGui::SliderInt4(name, valueptr, min, max, display_format, flags); break; }
		}
		ImGui::PopID();
		return SliderIntFlag;
	}

	bool FrameworkImGuiControlBase::SliderFloat(ImTypeSliderFloat type, const char* name, float* valueptr, float min, float max, const char* display_format, ImGuiSliderFlags flags) {
		ControlPushIDcount();
		bool SliderFloatFlag = false;
		switch (type) {
		case(SliderFloat1): { SliderFloatFlag = ImGui::SliderFloat(name, valueptr, min, max, display_format, flags);  break; }
		case(SliderFloat2): { SliderFloatFlag = ImGui::SliderFloat2(name, valueptr, min, max, display_format, flags); break; }
		case(SliderFloat3): { SliderFloatFlag = ImGui::SliderFloat3(name, valueptr, min, max, display_format, flags); break; }
		case(SliderFloat4): { SliderFloatFlag = ImGui::SliderFloat4(name, valueptr, min, max, display_format, flags); break; }
		}
		return SliderFloatFlag;
	}

	bool FrameworkImGuiControlBase::ColorEdit(ImTypeColorEdit type, const char* name, float* color, ImGuiColorEditFlags flags) {
		ControlPushIDcount();
		bool ColorEditFlag = false;
		switch (type) {
		case(ColorEdit3): { ColorEditFlag = ImGui::ColorEdit3(name, color, flags); break; }
		case(ColorEdit4): { ColorEditFlag = ImGui::ColorEdit4(name, color, flags); break; }
		}
		ImGui::PopID();
		return ColorEditFlag;
	}

	bool FrameworkImGuiControlBase::ColorPicker(ImTypeColorPicker type, const char* name, float* color, ImGuiColorEditFlags flags) {
		ControlPushIDcount();
		bool ColorPickerFlag = false;
		switch (type) {
		case(ColorPicker3): { return ImGui::ColorPicker3(name, color, flags); }
		case(ColorPicker4): { return ImGui::ColorPicker4(name, color, flags); }
		}
		ImGui::PopID();
		return ColorPickerFlag;
	}

	void FrameworkImGuiControlBase::DrawTable(const char* name, const std::vector<std::string>& headers, const std::vector<std::vector<std::string>>& data, ImGuiTableFlags flags) {
		// "data"必须为矩阵类型数组.
		if (headers.empty() || data.empty() || data[0].empty()) {
			LOGCONS::PushLogger(LOG_WARNING, SYSMD_LB_CONTROL, "draw_table data: headers == 0 || x == 0 || y == 0.");
			return;
		}
		if (headers.size() != data[0].size()) {
			LOGCONS::PushLogger(LOG_WARNING, SYSMD_LB_CONTROL, "draw_table data: headers != cols.");
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
		ImGui::PopID();
	}

	void FrameworkImGuiControlBase::ImGuiWindow(const char* name, std::function<void()> window_function, bool* p_open, ImGuiWindowFlags flags) {
		ControlPushIDcount();
		{
			ImGui::Begin(name, p_open, flags);
			window_function();
			ImGui::End();
		}
		ImGui::PopID();
	}

	void FrameworkImGuiControlBase::ImGuiWindowChild(
		const char*           name,
		std::function<void()> window_function,
		const ImVec2&         size,
		bool                  border,
		ImGuiWindowFlags      flags
	) {
		ControlPushIDcount();
		{
			ImGui::BeginChild(name, size, border, flags);
			window_function();
			ImGui::EndChild();
		}
		ImGui::PopID();
	}

	void FrameworkImGuiControlBase::ImGuiWindowTootip(std::function<void()> window_function) {
		ImGui::BeginTooltip();
		window_function();
		ImGui::EndTooltip();
	}

	void FrameworkImGuiControlBase::PushStyleVar(ImGuiStyleVar index, float value) {
		ImGui::PushStyleVar(index, value);
	}
	void FrameworkImGuiControlBase::PushStyleColor(ImGuiCol index, const ImVec4& color) {
		ImGui::PushStyleColor(index, color);
	}

	void FrameworkImGuiControlBase::PopStyleVar(uint32_t count) {
		ImGui::PopStyleVar((int)count);
	}
	void FrameworkImGuiControlBase::PopStyleColor(uint32_t count) {
		ImGui::PopStyleColor((int)count);
	}

	// **************************************** imgui tool_controls ****************************************
	
	void FrameworkImGuiControlBase::ToolDrawRectangle(const ImVec2& position, const ImVec2& size, const ImVec4& color, const float& linewidth) {
		// draw line_rectangle.
		ImGui::GetWindowDrawList()->AddRect(
			ImVec2(ImGui::GetWindowPos().x + position.x, ImGui::GetWindowPos().y + position.y),
			ImVec2(ImGui::GetWindowPos().x + position.x + size.x, ImGui::GetWindowPos().y + position.y + size.y),
			IM_COL32(ImU32(color.x * 255.0f), ImU32(color.y * 255.0f), ImU32(color.z * 255.0f), ImU32(color.w * 255.0f)),
			ImGui::GetStyle().FrameRounding,
			NULL,
			linewidth
		);
	}

	void FrameworkImGuiControlBase::ToolDrawRectangleFill(const ImVec2& position, const ImVec2& size, const ImVec4& color) {
		// draw fill_rectangle.
		ImGui::GetWindowDrawList()->AddRectFilled(
			ImVec2(ImGui::GetWindowPos().x + position.x, ImGui::GetWindowPos().y + position.y),
			ImVec2(ImGui::GetWindowPos().x + position.x + size.x, ImGui::GetWindowPos().y + position.y + size.y),
			IM_COL32(ImU32(color.x * 255.0f), ImU32(color.y * 255.0f), ImU32(color.z * 255.0f), ImU32(color.w * 255.0f)),
			ImGui::GetStyle().FrameRounding,
			NULL
		);
	}

	bool FrameworkImGuiControlBase::ToolHoverRectangle(const ImVec2& position, const ImVec2& size) {
		return ImGui::IsMouseHoveringRect(
			ImVec2(ImGui::GetWindowPos().x + position.x, ImGui::GetWindowPos().y + position.y),
			ImVec2(ImGui::GetWindowPos().x + position.x + size.x, ImGui::GetWindowPos().y + position.y + size.y)
		);
	}

	void FrameworkImGuiControlBase::ToolDrawTriangleFill(const ImVec2& position, const ImVec2& offset1, const ImVec2& offset2, const ImVec4& color) {
		// draw fill_triangle.
		ImGui::GetWindowDrawList()->AddTriangleFilled(
			ImVec2(ImGui::GetWindowPos().x + position.x, ImGui::GetWindowPos().y + position.y),
			ImVec2(ImGui::GetWindowPos().x + position.x + offset1.x, ImGui::GetWindowPos().y + position.y + offset1.y),
			ImVec2(ImGui::GetWindowPos().x + position.x + offset2.x, ImGui::GetWindowPos().y + position.y + offset2.y),
			IM_COL32(ImU32(color.x * 255.0f), ImU32(color.y * 255.0f), ImU32(color.z * 255.0f), ImU32(color.w * 255.0f))
		);
	}

	void FrameworkImGuiControlBase::ToolDrawImageUVscale(const uint32_t& TextureHandle, const ImVec2& position, const ImVec2& size, const float& scale) {
		const float& tmp_scale = scale < 0.0f ? 0.0f : scale;
		// draw image(scale).
		ImGui::GetWindowDrawList()->AddImage(
			CVT_IMPTR(TextureHandle),
			ImVec2(ImGui::GetWindowPos().x + position.x, ImGui::GetWindowPos().y + position.y),
			ImVec2(ImGui::GetWindowPos().x + position.x + size.x, ImGui::GetWindowPos().y + position.y + size.y),
			ImVec2(0.5f - tmp_scale / 2.0f, 0.5f - tmp_scale / 2.0f),
			ImVec2(0.5f + tmp_scale / 2.0f, 0.5f + tmp_scale / 2.0f)
		);
	}
}