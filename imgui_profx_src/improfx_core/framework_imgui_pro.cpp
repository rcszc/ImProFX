// framework_imgui_pro.
#include "framework_imgui.h"

using namespace std;
using namespace LOGCONS;

namespace ImGuiPro {

	void FullScreenBackground(uint32_t texture_hd, bool draw_fps) {
		ImGuiIO& io = ImGui::GetIO();

		ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
		ImGui::SetNextWindowSize(io.DisplaySize);
		ImGui::Begin("0x7F8A6D4B2E01F3C9", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs);

		ImGui::SetCursorPos(ImVec2(0.0f, 0.0f));
		ImGui::Image((void*)(intptr_t)texture_hd, io.DisplaySize);

		ImGui::SetCursorPos(ImVec2(IMGUI_ITEM_SPC, IMGUI_ITEM_SPC));
		if (draw_fps)
			ImGui::Text("GUI RENDER FPS: %0.2f", ImGui::GetIO().Framerate);
		ImGui::End();
	}

	void FullWindowBackground(uint32_t texture_hd, ImVec2 uv_cropping) {
		ImGui::SetCursorPos(ImVec2(0.0f, 0.0f));
		ImGui::Image(
			CVT_IMPTR(texture_hd), 
			ImVec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y - IMGUI_ITEM_SPC),
			ImVec2(0.5f - uv_cropping.x / 2.0f, 0.5f - uv_cropping.y / 2.0f),
			ImVec2(0.5f + uv_cropping.x / 2.0f, 0.5f + uv_cropping.y / 2.0f)
		);
		ImGui::SetCursorPos(ImVec2(IMGUI_ITEM_SPC, IMGUI_ITEM_SPC * 4.0f));
	}

	void BeginBg(const char* name, uint32_t texture_hd, ImVec2 size, ImVec2 uv_cropping, bool* p_open, ImGuiWindowFlags flags) {
		ImGui::SetNextWindowSize(size);
		ImGui::Begin(name, p_open, flags | ImGuiWindowFlags_NoResize);

		// ImVec2(0.0f, 0.0f) - ImVec2(limit.x, limit.y)
		ImGui::SetCursorPos(ImVec2(0.0f, 0.0f));
		ImGui::Image(
			CVT_IMPTR(texture_hd), 
			ImVec2(size.x, size.y - IMGUI_ITEM_SPC),
			ImVec2(0.5f - uv_cropping.x / 2.0f, 0.5f - uv_cropping.y / 2.0f),
			ImVec2(0.5f + uv_cropping.x / 2.0f, 0.5f + uv_cropping.y / 2.0f)
		);
		ImGui::SetCursorPos(ImVec2(8.0f, 8.0f * 4.0f));
	}

	float ItemCenteredCalc(float width) {
		return ImGui::GetWindowSize().x / 2.0f - width / 2.0f - IMGUI_ITEM_SPC;
	}

	void TextCentered(const char* text, ImVec4 color) {
		float indent_value = ImGui::GetWindowSize().x / 2.0f - ImGui::CalcTextSize(text).x / 2.0f - IMGUI_ITEM_SPC;

		ImGui::Indent(indent_value);
		ImGui::TextColored(color, text);
		ImGui::Unindent(indent_value);
	}

	bool ButtonImageText(const char* name, uint32_t texture_hd, ImVec2 size, uint32_t id, ImVec2 uv_cropping) {
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

	void InputTextCentered(const char* name, char* buffer, size_t buffer_size, float width, bool& password, int id) {
		float offxpos = ImGuiPro::ItemCenteredCalc(width + IMGUI_ITEM_SPC * 8.0f);
		ImGui::Indent(offxpos);

		ImGui::PushID(id);
		ImGui::Checkbox("", &password);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(width);
		ImGui::InputText(name, buffer, buffer_size, ImGuiInputTextFlags_Password * password);
		ImGui::PopID();
		ImGui::Unindent(offxpos);
	}

	void MouseFloatingWindow(ImVec2 size, bool focus, function<void()> window_ui) {
		ImGuiIO& io = ImGui::GetIO();

		ImGui::SetNextWindowPos(ImVec2(io.MousePos.x + 10.0f, io.MousePos.y + 12.0f), ImGuiCond_Always);
		ImGui::SetNextWindowSize(size, ImGuiCond_Always);

		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.16f, 0.16f, 0.16f, 0.895f));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 7.2f);

		ImGui::Begin("", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs);

		if (focus)
			ImGui::SetWindowFocus();

		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.72f, 0.72f, 1.0f, 0.92f));
		window_ui();
		ImGui::PopStyleColor();

		ImGui::End();
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();
	}

	void AnalogLED(bool state, ImVec4 h_color, ImVec4 l_color, ImVec2 size) {
		if (state) ImGui::ColorButton("", h_color, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoTooltip, size);
		else       ImGui::ColorButton("", l_color, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoTooltip, size);
	}

	void PushStyleColor(ImGuiCol index, Vector4T<float> color) { 
		ImGui::PushStyleColor(
			index,
			ImVec4(color.vector_x, color.vector_y, color.vector_z, color.vector_w)
		);
	}

	void EndEnterLine() { ImGui::Separator(); ImGui::Text(""); }
}