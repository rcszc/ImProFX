// framework_imgui_base.

#include "framework_imgui_base.hpp"

using namespace std;
using namespace LOGCONS;

namespace SYSIMGUI_COUPLING {
	std::atomic<uint32_t> ImGuiControlUniqueIDcount(0);

	// ######################################## imgui general_controls ########################################

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
		return ImGui::Button(name, size);
	}

	bool FrameworkImGuiControlBase::ColorButton(const char* name, const ImVec4& color, const ImGuiColorEditFlags& flags, const ImVec2& size) {
		ControlPushIDcount();
		return ImGui::ColorButton(name, color, flags, size);
	}

	bool FrameworkImGuiControlBase::Checkbox(const char* name, bool* flagptr) {
		ControlPushIDcount();
		return ImGui::Checkbox(name, flagptr);
	}

	bool FrameworkImGuiControlBase::RadioButton(const char* name, bool active) {
		ControlPushIDcount();
		return ImGui::RadioButton(name, active);
	}

	bool FrameworkImGuiControlBase::InputText(const char* name, char* buffer, size_t size, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data) {
		ControlPushIDcount();
		return ImGui::InputText(name, buffer, size, flags, callback, user_data);
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
		return ImGui::InputTextMultiline(name, buf, buf_size, size, flags, callback, user_data);
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
		return ImGui::ImageButton(name, user_texture_id, size, uv0, uv1, tint_col, border_col);
	}

	bool FrameworkImGuiControlBase::InputInt(ImTypeInputInt type, const char* name, int* valueptr, int step, int step_fast, ImGuiInputTextFlags flags) {
		ControlPushIDcount();
		switch (type) {
		case(InputInt1): { return ImGui::InputInt(name, valueptr, step, step_fast, flags); }
		case(InputInt2): { return ImGui::InputInt2(name, valueptr, flags); }
		case(InputInt3): { return ImGui::InputInt3(name, valueptr, flags); }
		case(InputInt4): { return ImGui::InputInt4(name, valueptr, flags); }
		default: return false;
		}
	}

	bool FrameworkImGuiControlBase::InputFloat(ImTypeInputFloat type, const char* name, float* valueptr, float step, float step_fast, const char* format, ImGuiInputTextFlags flags) {
		ControlPushIDcount();
		switch (type) {
		case(InputFloat1): { return ImGui::InputFloat(name, valueptr, step, step_fast, format, flags); }
		case(InputFloat2): { return ImGui::InputFloat2(name, valueptr, format, flags); }
		case(InputFloat3): { return ImGui::InputFloat3(name, valueptr, format, flags); }
		case(InputFloat4): { return ImGui::InputFloat4(name, valueptr, format, flags); }
		default: return false;
		}
	}

	bool FrameworkImGuiControlBase::SliderInt(ImTypeSliderInt type, const char* name, int* valueptr, int min, int max, const char* display_format, ImGuiSliderFlags flags) {
		ControlPushIDcount();
		switch (type) {
		case(SliderInt1): { return ImGui::SliderInt(name, valueptr, min, max, display_format, flags);  }
		case(SliderInt2): { return ImGui::SliderInt2(name, valueptr, min, max, display_format, flags); }
		case(SliderInt3): { return ImGui::SliderInt3(name, valueptr, min, max, display_format, flags); }
		case(SliderInt4): { return ImGui::SliderInt4(name, valueptr, min, max, display_format, flags); }
		default: return false;
		}
	}

	bool FrameworkImGuiControlBase::SliderFloat(ImTypeSliderFloat type, const char* name, float* valueptr, float min, float max, const char* display_format, ImGuiSliderFlags flags) {
		ControlPushIDcount();
		switch (type) {
		case(SliderFloat1): { return ImGui::SliderFloat(name, valueptr, min, max, display_format, flags);  }
		case(SliderFloat2): { return ImGui::SliderFloat2(name, valueptr, min, max, display_format, flags); }
		case(SliderFloat3): { return ImGui::SliderFloat3(name, valueptr, min, max, display_format, flags); }
		case(SliderFloat4): { return ImGui::SliderFloat4(name, valueptr, min, max, display_format, flags); }
		default: return false;
		}
	}

	bool FrameworkImGuiControlBase::ColorEdit(ImTypeColorEdit type, const char* name, float* color, ImGuiColorEditFlags flags) {
		ControlPushIDcount();
		switch (type) {
		case(ColorEdit3): { return ImGui::ColorEdit3(name, color, flags); }
		case(ColorEdit4): { return ImGui::ColorEdit4(name, color, flags); }
		default: return false;
		}
	}

	bool FrameworkImGuiControlBase::ColorPicker(ImTypeColorPicker type, const char* name, float* color, ImGuiColorEditFlags flags) {
		ControlPushIDcount();
		switch (type) {
		case(ColorPicker3): { return ImGui::ColorPicker3(name, color, flags); }
		case(ColorPicker4): { return ImGui::ColorPicker4(name, color, flags); }
		default: return false;
		}
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
	}

	void FrameworkImGuiControlBase::ImGuiWindow(const char* name, std::function<void()> window_function, bool* p_open, ImGuiWindowFlags flags) {
		ControlPushIDcount();
		ImGui::Begin(name, p_open, flags);
		window_function();
		ImGui::End();
	}

	void FrameworkImGuiControlBase::ImGuiWindowChild(
		const char*           name,
		std::function<void()> window_function,
		const ImVec2&         size,
		bool                  border,
		ImGuiWindowFlags      flags
	) {
		ControlPushIDcount();
		ImGui::BeginChild(name, size, border, flags);
		window_function();
		ImGui::End();
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

	// ######################################## imgui tool_controls ########################################
	
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