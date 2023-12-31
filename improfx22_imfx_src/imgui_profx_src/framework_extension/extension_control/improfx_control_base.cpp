// improfx_control_base. RCSZ.

#include "improfx_control_base.h"

// control_base: imgui window_draw_list.
namespace ImControlBase {

#define ICB_ZEROLIMIT(high, value) high - value < 0.0f ? 0.0f : high - value && high - value > 1.0f ? 1.0f : high - value
	ImVec4 ExtColorBrightnesScale(const ImVec4& color, float value) {
		return ImVec4(ICB_ZEROLIMIT(color.x, value), ICB_ZEROLIMIT(color.y, value), ICB_ZEROLIMIT(color.z, value), ICB_ZEROLIMIT(color.w, value));
	}

	float ExtItemCenteredCalc(const float& width) {
		return ImGui::GetWindowSize().x / 2.0f - width / 2.0f;
	}

	void ExtDrawLine(const ImVec2& point0, const ImVec2& point1, const ImVec4& color, float linewidth) {
		// draw line_segment.
		ImGui::GetWindowDrawList()->AddLine(
			ImVec2(ImGui::GetWindowPos().x + point0.x, ImGui::GetWindowPos().y + point0.y),
			ImVec2(ImGui::GetWindowPos().x + point1.x, ImGui::GetWindowPos().y + point1.y),
			IMVEC4_CVT_COLU32(color),
			linewidth
		);
	}

#define ICB_FMTLEN_CHARARRY 1024
	void ExtDrawText(const ImVec2& position, const ImVec4& color, const char* text, ...) {
		char LoggerStrTemp[ICB_FMTLEN_CHARARRY] = {};

		va_list ParamArgs;
		va_start(ParamArgs, text);
		vsnprintf(LoggerStrTemp, ICB_FMTLEN_CHARARRY, text, ParamArgs);
		va_end(ParamArgs);

		ImGui::GetWindowDrawList()->AddText(
			ImVec2(ImGui::GetWindowPos().x + position.x, ImGui::GetWindowPos().y + position.y),
			IMVEC4_CVT_COLU32(color),
			LoggerStrTemp
		);
	}

	inline void ExtDrawRectangle(const ImVec2& position, const ImVec2& size, const ImVec4& color, float linewidth) {
		// draw line_rectangle.
		ImGui::GetWindowDrawList()->AddRect(
			ImVec2(ImGui::GetWindowPos().x + position.x, ImGui::GetWindowPos().y + position.y),
			ImVec2(ImGui::GetWindowPos().x + position.x + size.x, ImGui::GetWindowPos().y + position.y + size.y),
			IMVEC4_CVT_COLU32(color),
			ImGui::GetStyle().FrameRounding,
			NULL,
			linewidth
		);
	}

	void ExtDrawRectangleFill(const ImVec2& position, const ImVec2& size, const ImVec4& color) {
		// draw fill_rectangle.
		ImGui::GetWindowDrawList()->AddRectFilled(
			ImVec2(ImGui::GetWindowPos().x + position.x, ImGui::GetWindowPos().y + position.y),
			ImVec2(ImGui::GetWindowPos().x + position.x + size.x, ImGui::GetWindowPos().y + position.y + size.y),
			IMVEC4_CVT_COLU32(color),
			ImGui::GetStyle().FrameRounding,
			NULL
		);
	}

	void ExtDrawCircleFill(const ImVec2& position, float size, const ImVec4& color) {
		// draw fill_circle.
		ImGui::GetWindowDrawList()->AddCircleFilled(
			ImVec2(ImGui::GetWindowPos().x + position.x, ImGui::GetWindowPos().y + position.y),
			size,
			IMVEC4_CVT_COLU32(color)
		);
	}

	void ExtDrawTriangleFill(const ImVec2& position, const ImVec2& offset1, const ImVec2& offset2, const ImVec4& color) {
		// draw fill_triangle.
		ImGui::GetWindowDrawList()->AddTriangleFilled(
			ImVec2(ImGui::GetWindowPos().x + position.x, ImGui::GetWindowPos().y + position.y),
			ImVec2(ImGui::GetWindowPos().x + position.x + offset1.x, ImGui::GetWindowPos().y + position.y + offset1.y),
			ImVec2(ImGui::GetWindowPos().x + position.x + offset2.x, ImGui::GetWindowPos().y + position.y + offset2.y),
			IMVEC4_CVT_COLU32(color)
		);
	}
}