// framework_imgui_pro.
#include <regex>

#include "framework_imgui.h"

using namespace std;
using namespace LOGCONS;

ImVec4 color_grayscale(ImVec4 incolor, float sub_h) {
	return ImVec4(incolor.x - sub_h, incolor.y - sub_h, incolor.z - sub_h, incolor.w - sub_h);
}

namespace basic_drawlist_comp {

	void draw_rectangle(const ImVec2& position, const ImVec2& size, const ImVec4& color, const float& linewidth) {
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

	void draw_rectangle_fill(const ImVec2& position, const ImVec2& size, const ImVec4& color) {
		// draw fill_rectangle.
		ImGui::GetWindowDrawList()->AddRectFilled(
			ImVec2(ImGui::GetWindowPos().x + position.x, ImGui::GetWindowPos().y + position.y),
			ImVec2(ImGui::GetWindowPos().x + position.x + size.x, ImGui::GetWindowPos().y + position.y + size.y),
			IM_COL32(ImU32(color.x * 255.0f), ImU32(color.y * 255.0f), ImU32(color.z * 255.0f), ImU32(color.w * 255.0f)),
			ImGui::GetStyle().FrameRounding,
			NULL
		);
	}

	void draw_image_uvscale(const uint32_t& texture_hd, const ImVec2& position, const ImVec2& size, const float& scale) {
		const float& tmp_scale = scale < 0.0f ? 0.0f : scale;
		// draw image(scale).
		ImGui::GetWindowDrawList()->AddImage(
			CVT_IMPTR(texture_hd),
			ImVec2(ImGui::GetWindowPos().x + position.x, ImGui::GetWindowPos().y + position.y),
			ImVec2(ImGui::GetWindowPos().x + position.x + size.x, ImGui::GetWindowPos().y + position.y + size.y),
			ImVec2(0.5f - tmp_scale / 2.0f, 0.5f - tmp_scale / 2.0f),
			ImVec2(0.5f + tmp_scale / 2.0f, 0.5f + tmp_scale / 2.0f)
		);
	}

	void draw_text_scale(const char* text, const float& scale, const ImVec4& color) {
		ImGui::SetWindowFontScale(scale);
		ImGui::TextColored(color, text);
	}
}

namespace ImGuiPro {

	void FullScreenBackground(const uint32_t& texture_hd, const bool& draw_fps) {
		ImGuiIO& io = ImGui::GetIO();

		ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
		ImGui::SetNextWindowSize(io.DisplaySize);
		ImGui::Begin("0x7F8A6D4B2E01F3C9", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs);

		ImGui::SetCursorPos(ImVec2(0.0f, 0.0f));
		ImGui::Image(CVT_IMPTR(texture_hd), io.DisplaySize);

		ImGui::SetCursorPos(ImVec2(IMGUI_ITEM_SPC, IMGUI_ITEM_SPC));
		if (draw_fps)
			ImGui::Text("GUI RENDER FPS: %0.2f", ImGui::GetIO().Framerate);
		ImGui::End();
	}

	void FullWindowBackground(const uint32_t& texture_hd, const ImVec2& uv_cropping, const float& yoffset) {
		ImGui::SetCursorPos(ImVec2(0.0f, ImGui::GetScrollY()));
		ImGui::Image(
			CVT_IMPTR(texture_hd),
			ImVec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y - IMGUI_ITEM_SPC + yoffset),
			ImVec2(0.5f - uv_cropping.x / 2.0f, 0.5f - uv_cropping.y / 2.0f),
			ImVec2(0.5f + uv_cropping.x / 2.0f, 0.5f + uv_cropping.y / 2.0f)
		);
		ImGui::SetCursorPos(ImVec2(IMGUI_ITEM_SPC, IMGUI_ITEM_SPC * 4.0f));
	}

	void BeginBackground(
		const char*      name,
		const uint32_t&  texture_hd,
		const ImVec2&    size,
		const ImVec2&    uv_cropping,
		bool*            p_open,
		ImGuiWindowFlags flags
	) {
		ImGui::SetNextWindowSize(size);
		ImGui::Begin(name, p_open, flags | ImGuiWindowFlags_NoResize);

		// ImVec2(0.0f, 0.0f) - ImVec2(limit.x, limit.y)
		ImGui::SetCursorPos(ImVec2(0.0f, ImGui::GetScrollY()));
		ImGui::Image(
			CVT_IMPTR(texture_hd),
			ImVec2(size.x, size.y - IMGUI_ITEM_SPC),
			ImVec2(0.5f - uv_cropping.x / 2.0f, 0.5f - uv_cropping.y / 2.0f),
			ImVec2(0.5f + uv_cropping.x / 2.0f, 0.5f + uv_cropping.y / 2.0f)
		);
		ImGui::SetCursorPos(ImVec2(IMGUI_ITEM_SPC, IMGUI_ITEM_SPC * 4.0f));
	}

	float ItemCalcCentered(const float& width) {
		return ImGui::GetWindowSize().x / 2.0f - width / 2.0f - IMGUI_ITEM_SPC;
	}

	void TextTitleP1(const char* text, const float& child_winx, const ImVec4& color, const bool& border) {
		ImGui::BeginChild(text, ImVec2(child_winx, 55.0f), border);
		basic_drawlist_comp::draw_text_scale(text, 2.2f, color);
		ImGui::EndChild();
	}

	void TextTitleP2(const char* text, const float& child_winx, const ImVec4& color, const bool& border) {
		ImGui::BeginChild(text, ImVec2(child_winx, 50.0f), border);
		basic_drawlist_comp::draw_text_scale(text, 1.72f, color);
		ImGui::EndChild();
	}

	void TextTitleP3(const char* text, const float& child_winx, const ImVec4& color, const bool& border) {
		ImGui::BeginChild(text, ImVec2(child_winx, 42.0f), border);
		basic_drawlist_comp::draw_text_scale(text, 1.48f, color);
		ImGui::EndChild();
	}

	void TextCentered(const char* text, const ImVec4& color) {
		float indent_value = ImGui::GetWindowSize().x / 2.0f - ImGui::CalcTextSize(text).x / 2.0f - IMGUI_ITEM_SPC;

		ImGui::Indent(indent_value);
		ImGui::TextColored(color, text);
		ImGui::Unindent(indent_value);
	}

	void RoundImage(const uint32_t& texture_hd, const ImVec2& position, const ImVec2& size, const float& round, const ImVec2& uv_cropping) {
		ImGui::GetWindowDrawList()->AddImageRounded(
			CVT_IMPTR(texture_hd),
			ImVec2(ImGui::GetWindowPos().x + position.x, ImGui::GetWindowPos().y + position.y),
			ImVec2(ImGui::GetWindowPos().x + position.x + size.x, ImGui::GetWindowPos().y + position.y + size.y),
			ImVec2(0.5f - uv_cropping.x / 2.0f, 0.5f - uv_cropping.y / 2.0f),
			ImVec2(0.5f + uv_cropping.x / 2.0f, 0.5f + uv_cropping.y / 2.0f),
			IM_COL32_WHITE,
			round
		);
	}

	bool ButtonImageText(
		const char*     name,
		const uint32_t& texture_hd,
		const ImVec2&   size,
		const uint32_t& id,
		const ImVec2&   uv_cropping
	) {
		ImVec2 tmppos = ImGui::GetCursorPos();

		ImGui::PushID(id);
		bool buttonflag = ImGui::ImageButton(
			CVT_IMPTR(texture_hd),
			size,
			ImVec2(0.5f - uv_cropping.x / 2.0f, 0.5f - uv_cropping.y / 2.0f),
			ImVec2(0.5f + uv_cropping.x / 2.0f, 0.5f + uv_cropping.y / 2.0f)
		);
		ImGui::PopID();
		ImVec2 tmppos_s = ImGui::GetCursorPos();

		float offx = size.x / 2.0f - ImGui::CalcTextSize(name).x / 2.0f;
		float offy = size.y / 2.0f - ImGui::CalcTextSize(name).y / 2.0f;

		ImGui::SetCursorPos(ImVec2(tmppos.x + offx, tmppos.y + offy));
		ImGui::Text(name);
		ImGui::SetCursorPos(tmppos_s);

		return buttonflag;
	}

	void FloatingWindowValue(const float& value, const ImVec2& limit, const ImVec4& color) {
		float length = abs(limit.x) + abs(limit.y);

		ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.1f, 0.5f, 0.5f, 0.32f));
		ImGui::BeginTooltip();
		{
			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, color);
			ImGui::PushStyleColor(ImGuiCol_FrameBg, color_grayscale(color, 0.28f));

			ImGui::TextColored(color, "%0.3f [ %0.2f, %0.2f ]", value, limit.x, limit.y);
			ImGui::ProgressBar(abs(value - limit.x) / length);

			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
		}
		ImGui::EndTooltip();
		ImGui::PopStyleColor();
	}

	void FloatingWindowImage(const uint32_t& texture_hd, const ImVec2& size) {
		ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.1f, 0.5f, 0.5f, 0.32f));
		ImGui::BeginTooltip();
		{
			ImGui::Image(CVT_IMPTR(texture_hd), size);
		}
		ImGui::EndTooltip();
		ImGui::PopStyleColor();
	}

	void InputTextWireframe(const char* name, const inputchar& data, const float& width, const ImVec4& color) {
		const float linewidth = 2.8f;
		ImVec2 temp_pos = ImGui::GetCursorPos();
		
		basic_drawlist_comp::draw_rectangle(
			ImVec2(temp_pos.x - linewidth / 2.0f, temp_pos.y - linewidth / 2.0f),
			ImVec2(width + linewidth, 25.0f + linewidth),
			color
		);
		ImGui::SetCursorPos(temp_pos);

		ImGui::SetNextItemWidth(width);
		ImGui::InputText(name, data.input_buffer_pointer, data.input_buffer_size);
	}

	void InputTextStatusPrompt(const char* name, const inputchar& data, const float& width, const bool& state, const ImVec4& h_color, const ImVec4& l_color) {
		ImVec4 statcolor = {};

		if (state) statcolor = h_color;
		else       statcolor = l_color;

		InputTextWireframe(name, data, width, statcolor);
	}

	void InputCenteredText(const char* name, inputchar data, const float& width) {
		float offxpos = ImGuiPro::ItemCalcCentered(width);
		ImGui::Indent(offxpos);

		ImGui::SetNextItemWidth(width);
		ImGui::InputText(name, data.input_buffer_pointer, data.input_buffer_size);

		ImGui::Unindent(offxpos);
	}

	void InputCenteredTextPsw(const char* name, inputchar data, const float& width, bool password) {
		float offxpos = ImGuiPro::ItemCalcCentered(width + IMGUI_ITEM_SPC + 24.5f);
		ImGui::Indent(offxpos);

		ImGui::Checkbox("", &password);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(width);
		ImGui::InputText(name, data.input_buffer_pointer, data.input_buffer_size, ImGuiInputTextFlags_Password * password);

		ImGui::Unindent(offxpos);
	}

	void ViewTextMultiline(const char* name, string& text_str, const ImVec2& size) {
		ImGui::InputTextMultiline(name, &text_str[0], text_str.size(), size, ImGuiInputTextFlags_ReadOnly);
	}

	void ViewTextMultilineAnim(
		const char*   name,
		std::string&  text_str,
		float&        count,
		const bool&   dis,
		const ImVec2& size,
		const float&  speed
	) {
		count = count >= (float)text_str.size() ? (float)text_str.size() : count;
		ImGui::InputTextMultiline(name, &text_str.substr(0, (size_t)count)[0], text_str.size(), size, ImGuiInputTextFlags_ReadOnly);

		if (dis) count += 0.32f * speed;
		else     count = 0.0f;
	}

	bool DetePosScrollY(ImVec2 limit) {
		float scroll_posy = ImGui::GetScrollY();
		return scroll_posy > limit.x && scroll_posy < limit.y ? true : false;
	}

	void InterSliderFloat(
		const char*      name,
		Vector2T<float>& f_buffer,
		ImVec2           limit,
		float            trans_speed,
		const char*      format,
		ImGuiSliderFlags flags
	) {
		ImGui::SliderFloat(name, &f_buffer.vector_x, limit.x, limit.y, format, flags);
		f_buffer.vector_y += (f_buffer.vector_x - f_buffer.vector_y) * 0.032f * trans_speed;
	}

#define SMOOTH_YSTEP 72.0f
	void SmoothYSlide(Vector2T<float>& ypos, const float& speed) {
		// 聚焦当前窗口时生效.
		if (ImGui::IsWindowFocused()) {
			ImGui::SetScrollY(ypos.vector_y);

			if (ImGui::GetIO().MouseWheel < 0.0f) ypos.vector_x += SMOOTH_YSTEP;
			if (ImGui::GetIO().MouseWheel > 0.0f) ypos.vector_x -= SMOOTH_YSTEP;
			// slide y limit.
			float ymax = ImGui::GetScrollMaxY() - IMGUI_ITEM_SPC;

			ypos.vector_x = ypos.vector_x < 0.0f ? 0.0f : ypos.vector_x;
			ypos.vector_x = ypos.vector_x > ymax ? ymax : ypos.vector_x;

			ypos.vector_y += (ypos.vector_x - ypos.vector_y) * 0.075f * speed;
			ypos.vector_y = ypos.vector_y > ymax ? ymax : ypos.vector_y;
		}
	}

	void LndicatorLED(const bool& state, const ImVec2& size, const ImVec4& h_color, const ImVec4& l_color) {
		if (state) ImGui::ColorButton("", h_color, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoTooltip, size);
		else       ImGui::ColorButton("", l_color, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoTooltip, size);
	}

	void PushStyleColor(const ImGuiCol& index, const Vector4T<float>& color) {
		ImGui::PushStyleColor(
			index,
			ImVec4(color.vector_x, color.vector_y, color.vector_z, color.vector_w)
		);
	}

	void EndEnterLine() { ImGui::Text(""); ImGui::Separator(); ImGui::Text(""); }
}