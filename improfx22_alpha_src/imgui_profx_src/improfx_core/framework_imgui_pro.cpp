// framework_imgui_pro.
#include <regex>

#include "framework_imgui.h"

using namespace std;
using namespace LOGCONS;

inline ImVec4 color_gray_scale(const ImVec4& incolor, float sub_h) {
	return ImVec4(incolor.x - sub_h, incolor.y - sub_h, incolor.z - sub_h, incolor.w - sub_h);
}

namespace ImGuiPro {

	void FullScreenImWindowBg(const uint32_t& texture_hd, const bool& draw_fps) {
		ImGuiIO& ImIO = ImGui::GetIO();

		ImGui::SetNextWindowPos(ImVec2(-IMGUI_ITEM_SPC * 0.5f, 0.0f));
		ImGui::SetNextWindowSize(ImVec2(ImIO.DisplaySize.x + IMGUI_ITEM_SPC, ImIO.DisplaySize.y));
		ImGui::Begin("0x7F8A6D4B2E01F3C9", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBringToFrontOnFocus);

		ImGui::SetCursorPos(ImVec2(0.0f, 0.0f));
		ImGui::Image(CVT_IMPTR(texture_hd), ImVec2(ImIO.DisplaySize.x + IMGUI_ITEM_SPC * 0.5f, ImIO.DisplaySize.y));
		
		ImGui::SetCursorPos(ImVec2(IMGUI_ITEM_SPC, IMGUI_ITEM_SPC));
		if (draw_fps)
			ImGui::Text("GUI RENDER FPS: %0.2f", ImGui::GetIO().Framerate);
		ImGui::End();
	}

	void FrameworkImGuiControl::PRO_FullWindowBackground(uint32_t texture_hd, const ImVec2& uv_cropping, float yoffset) {
		SetCursorPos(ImVec2(0.0f, ImGui::GetScrollY()));
		Image(
			CVT_IMPTR(texture_hd),
			ImVec2(GetWindowSize().x, GetWindowSize().y - IMGUI_ITEM_SPC + yoffset),
			ImVec2(0.5f - uv_cropping.x / 2.0f, 0.5f - uv_cropping.y / 2.0f),
			ImVec2(0.5f + uv_cropping.x / 2.0f, 0.5f + uv_cropping.y / 2.0f)
		);
		SetCursorPos(ImVec2(IMGUI_ITEM_SPC, IMGUI_ITEM_SPC * 4.0f));
	}

	void FrameworkImGuiControl::PRO_ImGuiWindowBackground(
		const char*           name,
		const uint32_t&       texture_hd,
		const ImVec2&         size,
		std::function<void()> window_function,
		const ImVec2&         uv_cropping,
		bool*                 p_open,
		ImGuiWindowFlags      flags
	) {
		SetNextWindowSize(size);
		ImGuiWindow(name, [&]() {
			
			// ImVec2(0.0f, 0.0f) - ImVec2(limit.x, limit.y)
			SetCursorPos(ImVec2(0.0f, ImGui::GetScrollY()));
			Image(
				CVT_IMPTR(texture_hd),
				ImVec2(size.x, size.y - IMGUI_ITEM_SPC),
				ImVec2(0.5f - uv_cropping.x / 2.0f, 0.5f - uv_cropping.y / 2.0f),
				ImVec2(0.5f + uv_cropping.x / 2.0f, 0.5f + uv_cropping.y / 2.0f)
			);
			SetCursorPos(ImVec2(IMGUI_ITEM_SPC, IMGUI_ITEM_SPC * 4.0f));
			// calling function objects.
			window_function();
		}, p_open, flags | ImGuiWindowFlags_NoResize);
	}

	float FrameworkImGuiControl::PRO_ItemCalcCentered(float width) {
		return GetWindowSize().x / 2.0f - width / 2.0f - IMGUI_ITEM_SPC;
	}

	inline void WindowTextScale(const char* text, float scale, const ImVec4& color) {
		ImGui::SetWindowFontScale(scale);
		ImGui::TextColored(color, text);
	}

	void FrameworkImGuiControl::PRO_TextTitleP1(const char* text, float child_winx, const ImVec4& color, const bool& border) {
		// imgui child_window => scale => draw_text. const_height: 55.0f
		ImGuiWindowChild(text, [&]() { WindowTextScale(text, 2.2f, color); }, 
			ImVec2(child_winx, 55.0f), border
		);
	}

	void FrameworkImGuiControl::PRO_TextTitleP2(const char* text, float child_winx, const ImVec4& color, const bool& border) {
		// imgui child_window => scale => draw_text. const_height: 50.0f
		ImGuiWindowChild(text, [&]() { WindowTextScale(text, 1.8f, color); }, 
			ImVec2(child_winx, 50.0f), border
		);
	}

	void FrameworkImGuiControl::PRO_TextTitleP3(const char* text, float child_winx, const ImVec4& color, const bool& border) {
		// imgui child_window => scale => draw_text. const_height: 42.0f
		ImGuiWindowChild(text, [&]() { WindowTextScale(text, 1.48f, color); }, 
			ImVec2(child_winx, 42.0f), border
		);
	}

	void FrameworkImGuiControl::PRO_TextCentered(const char* text, const ImVec4& color) {
		float IndentValue = PRO_ItemCalcCentered(ImGui::CalcTextSize(text).x);
		// text_size => calc_centered => indent => draw_text => umindent.
		Indent(IndentValue);
		TextColored(color, text);
		Unindent(IndentValue);
	}

	void FrameworkImGuiControl::PRO_RoundImage(
		const uint32_t& texture_hd, 
		const ImVec2&   position, 
		const ImVec2&   size, 
		float           round, 
		const ImVec2&   uv_cropping
	) {
		ImGui::GetWindowDrawList()->AddImageRounded(
			CVT_IMPTR(texture_hd),
			ImVec2(GetWindowPos().x + position.x, GetWindowPos().y + position.y),
			ImVec2(GetWindowPos().x + position.x + size.x, GetWindowPos().y + position.y + size.y),
			ImVec2(0.5f - uv_cropping.x / 2.0f, 0.5f - uv_cropping.y / 2.0f),
			ImVec2(0.5f + uv_cropping.x / 2.0f, 0.5f + uv_cropping.y / 2.0f),
			IM_COL32_WHITE,
			round
		);
	}

	bool FrameworkImGuiControl::PRO_ButtonImageText(
		const char*     name,
		const uint32_t& texture_hd,
		const ImVec2&   size,
		const ImVec2&   uv_cropping
	) {
		ImVec2 PosTemp = GetCursorPos();

		bool ButtonStateFlag = ImageButton(
			name,
			CVT_IMPTR(texture_hd),
			size,
			ImVec2(0.5f - uv_cropping.x / 2.0f, 0.5f - uv_cropping.y / 2.0f),
			ImVec2(0.5f + uv_cropping.x / 2.0f, 0.5f + uv_cropping.y / 2.0f)
		);
		ImVec2 ReposTemp = GetCursorPos();

		float Xoffset = size.x / 2.0f - CalcTextSize(name).x / 2.0f;
		float Yoffset = size.y / 2.0f - CalcTextSize(name).y / 2.0f;

		SetCursorPos(ImVec2(PosTemp.x + Xoffset, PosTemp.y + Yoffset));
		Text(name);
		SetCursorPos(ReposTemp);

		return ButtonStateFlag;
	}

	void FrameworkImGuiControl::PRO_FloatingWindowValue(float value, const ImVec2& limit, const ImVec4& color) {
		float Length = abs(limit.x) + abs(limit.y);

		PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.1f, 0.5f, 0.5f, 0.32f));
		ImGuiWindowTootip(
			[&]() {
				PushStyleColor(ImGuiCol_PlotHistogram, color);
				PushStyleColor(ImGuiCol_FrameBg, color_gray_scale(color, 0.28f));

				TextColored(color, "%0.3f [ %0.2f, %0.2f ]", value, limit.x, limit.y);
				ProgressBar(abs(value - limit.x) / Length);

				PopStyleColor(2);
			});
		PopStyleColor();
	}

	void FrameworkImGuiControl::PRO_FloatingWindowImage(uint32_t texture_hd, const ImVec2& size) {
		PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.1f, 0.5f, 0.5f, 0.32f));
		ImGuiWindowTootip(
			[&]() {
				Image(CVT_IMPTR(texture_hd), size);
			});
		PopStyleColor();
	}

	void FrameworkImGuiControl::PRO_InputTextWireframe(const char* name, const InputCharPtr& data, float width, const ImVec4& color) {
		const float LineWidth = 2.8f;
		ImVec2 ReposTemp = GetCursorPos();
		
		ToolDrawRectangle(
			ImVec2(ReposTemp.x - LineWidth / 2.0f, ReposTemp.y - LineWidth / 2.0f),
			ImVec2(width + LineWidth, 25.0f + LineWidth),
			color
		);
		SetCursorPos(ReposTemp);

		SetNextItemWidth(width);
		InputText(name, data.InputBufferPointer, data.InputBufferSize);
	}

	void FrameworkImGuiControl::PRO_InputTextStatusPrompt(
		const char*         name, 
		const InputCharPtr& data, 
		float               width, 
		bool                state, 
		const ImVec4&       high_color, 
		const ImVec4&       low_color
	) {
		ImVec4 StateColor = {};

		if (state) StateColor = high_color;
		else       StateColor = low_color;

		PRO_InputTextWireframe(name, data, width, StateColor);
	}

	void FrameworkImGuiControl::PRO_InputCenteredText(const char* name, const InputCharPtr& data, float width) {
		float Xoffset = PRO_ItemCalcCentered(width);
		Indent(Xoffset);

		SetNextItemWidth(width);
		InputText(name, data.InputBufferPointer, data.InputBufferSize);

		Unindent(Xoffset);
	}

	void FrameworkImGuiControl::PRO_InputCenteredTextPsw(const char* name, const InputCharPtr& data, float width, bool password) {
		float Xoffset = PRO_ItemCalcCentered(width + IMGUI_ITEM_SPC + 24.5f);
		Indent(Xoffset);

		Checkbox("", &password);
		SameLine();
		SetNextItemWidth(width);
		InputText(name, data.InputBufferPointer, data.InputBufferSize, ImGuiInputTextFlags_Password * password);

		Unindent(Xoffset);
	}

	void FrameworkImGuiControl::PRO_ViewTextMultiline(const char* name, string& text_str, const ImVec2& size) {
		InputTextMultiline(name, &text_str[0], text_str.size(), size, ImGuiInputTextFlags_ReadOnly);
	}

	void FrameworkImGuiControl::PRO_ViewTextMultilineAnim(
		const char*   name,
		const string& text_str,
		float&        count,
		bool          dis,
		const ImVec2& size,
		float         speed
	) {
		count = count >= (float)text_str.size() ? (float)text_str.size() : count;
		InputTextMultiline(name, &text_str.substr(0, (size_t)count)[0], text_str.size(), size, ImGuiInputTextFlags_ReadOnly);

		if (dis) count += 0.32f * speed;
		else     count = 0.0f;
	}

	void FrameworkImGuiControl::PRO_CheckboxTriangle(const char* name, bool* p_flag) {
		ImVec2 DwPosition = GetCursorPos();
		ImVec2 DwSize = ImVec2(24.0f, 24.0f);
		ImVec4 DwColor = GetStyleColorVec4(ImGuiCol_FrameBg);

		ColorButton(name, DwColor, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoTooltip, DwSize);
		if (IsItemHovered() && IsMouseClicked(0))
			*p_flag = !*p_flag;
		if (*p_flag)
			ToolDrawTriangleFill(
				ImVec2(DwPosition.x + 4.0f, DwPosition.y + 4.0f),
				ImVec2(16.0f, 0.0),
				ImVec2(8.0f, 16.0f),
				ImVec4(0.0f, 1.0f, 1.0f, 1.0f)
			);
		SameLine();
		Text(name);
	}

	bool FrameworkImGuiControl::PRO_DetePosScrollY(ImVec2 limit) {
		float YposScroll = ImGui::GetScrollY();
		return YposScroll > limit.x && YposScroll < limit.y ? true : false;
	}

	void FrameworkImGuiControl::PRO_InterSliderFloat(
		const char*      name,
		Vector2T<float>& f_buffer,
		ImVec2           limit,
		float            trans_speed,
		const char*      format,
		ImGuiSliderFlags flags
	) {
		SliderFloat(SliderFloat1, name, &f_buffer.vector_x, limit.x, limit.y, format, flags);
		f_buffer.vector_y += (f_buffer.vector_x - f_buffer.vector_y) * 0.032f * trans_speed;
	}

#define SMOOTH_YSTEP 72.0f
	void FrameworkImGuiControl::PRO_SmoothYSlide(Vector2T<float>& ypos, float speed, bool wheel, bool focus) {
		if (IsWindowFocused() || !focus) {
			if (wheel) {
				if (GetImGuiIO().MouseWheel < 0.0f) ypos.vector_x += SMOOTH_YSTEP;
				if (GetImGuiIO().MouseWheel > 0.0f) ypos.vector_x -= SMOOTH_YSTEP;
			}
			// slide y limit.
			float YmaxposTemp = ImGui::GetScrollMaxY() - IMGUI_ITEM_SPC;

			ypos.vector_x = ypos.vector_x < 0.0f ? 0.0f : ypos.vector_x;
			ypos.vector_x = ypos.vector_x > YmaxposTemp ? YmaxposTemp : ypos.vector_x;

			ypos.vector_y += (ypos.vector_x - ypos.vector_y) * 0.075f * speed;
			ypos.vector_y = ypos.vector_y > YmaxposTemp ? YmaxposTemp : ypos.vector_y;

			ImGui::SetScrollY(ypos.vector_y);
		}
	}

	void FrameworkImGuiControl::PRO_LndicatorLED(bool state, const ImVec2& size, const ImVec4& high_color, const ImVec4& low_color) {
		if (state) ColorButton("", high_color, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoTooltip, size);
		else       ColorButton("", low_color, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoTooltip, size);
	}

	void FrameworkImGuiControl::PRO_EndEnterLine() { 
		Text(""); 
		ImGui::Separator(); 
		Text(""); 
	}
}