// improfx_control_menu. RCSZ. [20231220]
// ImGui: [ChildWindow(BeginChild_EndChild)], Animation Menu, Update: 20231226.

#include "improfx_control.h"

void SmoothMenuChildWindow::DrawMenuTypeRect(float rect_height, const ImVec4& color) {
	// fill selection_box.
	ImControlBase::ExtDrawRectangleFill(
		ImVec2(ImControlBase::ExtItemCenteredCalc(MenuBufferWidthType.y), MenuBufferYposType.y),
		ImVec2(MenuBufferWidthType.y, rect_height + 2.0f),
		color
	);
	ImGui::SetCursorPos(ImVec2(0.0f, 0.0f));
}

void SmoothMenuChildWindow::DrawMenuItemRect(float rect_height, const ImVec4& color) {
	// fill item_box.
	ImControlBase::ExtDrawRectangleFill(
		ImVec2(ImControlBase::ExtItemCenteredCalc(MenuBufferWidthItem.y), MenuBufferYposItem.y),
		ImVec2(MenuBufferWidthItem.y, rect_height + 2.0f),
		color
	);
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 1.2f);
	// fill item_location_box.
	ImControlBase::ExtDrawRectangleFill(
		ImVec2(ImGui::GetWindowSize().x - 12.0f, MenuBufferYposItem.y),
		ImVec2(IMGUI_ITEM_SPC, rect_height + 2.0f),
		color
	);
	// fill item_location_dir.
	ImControlBase::ExtDrawTriangleFill(
		ImVec2(IMGUI_ITEM_SPC * 2.0f, MenuBufferYposItem.y + (rect_height + 2.0f) * 0.5f),
		ImVec2(-rect_height / 2.0f, -rect_height / 2.0f),
		ImVec2(-rect_height / 2.0f, rect_height / 2.0f),
		color
	);
	ImGui::PopStyleVar();
	ImGui::SetCursorPos(ImVec2(0.0f, 0.0f));
}

void SmoothMenuChildWindow::MenuInterCalc(ImVec2& posy_calc, ImVec2& width_calc, float speed) {
	// inter: result = y + (x − y) * t.
	posy_calc.y += (posy_calc.x - posy_calc.y) * 0.072f * speed;
	width_calc.y += (width_calc.x - width_calc.y) * 0.072f * speed;
}

void SmoothMenuChildWindow::DrawMenuWindow(
	const char*                     name,
	const std::vector<std::string>& items,
	uint32_t&                       count,
	const ImVec4&                   color,
	const ImVec2&                   size,
	float                           speed,
	float                           textscale
) {
	ImGui::BeginChild(name, size);
	ImControlBase::ExtDrawRectangleFill(ImVec2(), size, ImControlBase::ExtColorBrightnesScale(color, 0.5f));
	ImGui::SetWindowFontScale(textscale);

	ImGui::PushStyleColor(ImGuiCol_Text, color);
	DrawMenuTypeRect(TextDrawHeight, ImControlBase::ExtColorBrightnesScale(color, 0.15f));
	DrawMenuItemRect(TextDrawHeight, ImControlBase::ExtColorBrightnesScale(color, 0.25f));

	// draw menu_items.
	for (size_t i = 0; i < items.size(); ++i) {

		ImVec2 ItemTextSize = ImGui::CalcTextSize(items[i].c_str());
		float DrawHeightPosition = ImGui::GetCursorPosY();

		ImGui::SetCursorPosX(ImControlBase::ExtItemCenteredCalc(ItemTextSize.x));
		ImGui::Text(items[i].c_str());

		if (ImGui::IsItemHovered()) {
			MenuBufferYposType.x = DrawHeightPosition - ImGui::GetScrollY();
			MenuBufferWidthType.x = ItemTextSize.x + IMGUI_ITEM_SPC * 2.0f;

			if (ImGui::GetMouseClickedCount(0))
				count = (uint32_t)i;
		}
		if (count == (uint32_t)i) {
			MenuBufferYposItem.x = DrawHeightPosition - ImGui::GetScrollY();
			MenuBufferWidthItem.x = ItemTextSize.x + IMGUI_ITEM_SPC * 2.0f;
		}
		TextDrawHeight = ItemTextSize.y;
	}
	MenuInterCalc(MenuBufferYposType, MenuBufferWidthType, speed);
	MenuInterCalc(MenuBufferYposItem, MenuBufferWidthItem, speed);

	ImGui::PopStyleColor();
	ImGui::EndChild();
}