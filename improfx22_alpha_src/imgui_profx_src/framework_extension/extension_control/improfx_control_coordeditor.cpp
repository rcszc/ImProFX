// improfx_control_coordeditor. RCSZ. [20231224]
// ImGui: [ChildWindow(BeginChild_EndChild)], Animation Groups Editor, Update: 20231226.

#include "improfx_control.h"

#define LIMIT_CLAMP(value, min, max) ((value) < (min) ? (min) : ((value) > (max) ? (max) : (value)))

constexpr float RulerScaleValue  = 5.0f;
constexpr float PoswinCircleSize = 20.0f;

inline ImVec2 EXCHPOINTS_MIN(const ImVec2& PointA, const ImVec2& PointB) {
	return ImVec2(fmin(PointA.x, PointB.x), fmin(PointA.y, PointB.y));
}

inline ImVec2 EXCHPOINTS_MAX(const ImVec2& PointA, const ImVec2& PointB) {
	return ImVec2(fmax(PointA.x, PointB.x), fmax(PointA.y, PointB.y));
}

void CoordSystemEditorWindow::DrawCoordinateXRuler(const ImVec2& limit, const ImVec4& color, float length, float center, float scale, float ruler) {
	for (float i = limit.x * scale + center; i < limit.y * scale + center + 0.1f; i += ruler * scale) {
		// ruler main scale.
		ImControlBase::ExtDrawLine(ImVec2(i, 0.0f), ImVec2(i, length), color, 1.8f);
		if (scale > 0.72f)
			for (float j = 0.0f; j < ruler / RulerScaleValue; j += 1.0f)
				ImControlBase::ExtDrawLine(ImVec2(i + RulerScaleValue * scale * j, 0.0f), ImVec2(i + RulerScaleValue * scale * j, length * 0.58f), color, 1.2f);
	}
}

void CoordSystemEditorWindow::DrawCoordinateYRuler(const ImVec2& limit, const ImVec4& color, float length, float center, float scale, float ruler) {
	for (float i = limit.x * scale + center; i < limit.y * scale + center + 0.1f; i += ruler * scale) {
		// ruler main scale.
		ImControlBase::ExtDrawLine(ImVec2(0.0f, i), ImVec2(length, i), color, 1.8f);
		if (scale > 0.72f)
			for (float j = 0.0f; j < ruler / RulerScaleValue; j += 1.0f)
				ImControlBase::ExtDrawLine(ImVec2(0.0f, i + RulerScaleValue * scale * j), ImVec2(length * 0.58f, i + RulerScaleValue * scale * j), color, 1.2f);
	}
}

void CoordSystemEditorWindow::DrawCoordinateLines(const ImVec4& color, const ImVec2& mouse, const ImVec2& win_size) {
	ImControlBase::ExtDrawLine(ImVec2(mouse.x, 0.0f), ImVec2(mouse.x, mouse.y), color, 1.2f);
	ImControlBase::ExtDrawLine(ImVec2(0.0f, mouse.y), ImVec2(mouse.x, mouse.y), color, 1.2f);
}

void CoordSystemEditorWindow::DrawGrid(
	const ImVec2& limitx, const ImVec2& limity, const ImVec4& color, const ImVec2& pos, const ImVec2& win_size,
	float scale, float ruler
) {
	ImVec2 DrawPosition(pos.x + win_size.x * 0.5f, pos.y + win_size.y * 0.5f);

	for (float i = limitx.x * scale + DrawPosition.x; i < limitx.y * scale + DrawPosition.x + 0.1f; i += ruler * scale)
		ImControlBase::ExtDrawLine(ImVec2(i, limity.x * scale + DrawPosition.y), ImVec2(i, limity.y * scale + DrawPosition.y), color, 1.8f);
	for (float i = limity.x * scale + DrawPosition.y; i < limity.y * scale + DrawPosition.y + 0.1f; i += ruler * scale)
		ImControlBase::ExtDrawLine(ImVec2(limitx.x * scale + DrawPosition.x, i), ImVec2(limitx.y * scale + DrawPosition.x, i), color, 1.8f);

	ImControlBase::ExtDrawCircleFill(DrawPosition, 3.2f, ImControlBase::ExtColorBrightnesScale(color, -0.16f));
}

void CoordSystemEditorWindow::PositioningWindow(
	ImVec2& index, const ImVec2& limitx, const ImVec2& limity, const ImVec4& color, ImTextureID texture,
	float size, float scale
) {
	ImVec2 WindowCenter(size * 0.5f, size * 0.5f);
	ImVec2 LimitMin(0.0f - WindowCenter.x, 0.0f - WindowCenter.y);
	// LimitMax = WindowCenter;

	ImGui::BeginChild("@POSWIN", ImVec2(size, size));
	{
		PosWinCurrPosition.x = size * (limitx.y * scale - index.x) / (limitx.y * scale - limitx.x * scale) - WindowCenter.x;
		PosWinCurrPosition.y = size * (limity.y * scale - index.y) / (limity.y * scale - limity.x * scale) - WindowCenter.y;

		ImVec2 WindowMousePos = IMVEC2_SUB2(IMVEC2_SUB2(ImGui::GetMousePos(), ImGui::GetWindowPos()), WindowCenter);   // 鼠标相对子窗口中心坐标.
		ImVec2 Proportion((limitx.y * scale - limitx.x * scale) / size, (limity.y * scale - limity.x * scale) / size); // 映射到网格窗口坐标缩放比例.

		ImControlBase::ExtDrawRectangleFill(ImVec2(0.0f, 0.0f), ImVec2(size, size), ImControlBase::ExtColorBrightnesScale(EditorColorSystem, 0.64f));
		if (texture)
			ImGui::Image(texture, ImVec2(size, size));

		ImControlBase::ExtDrawLine(ImVec2(WindowCenter.x, 0.0f), ImVec2(WindowCenter.x, size), ImControlBase::ExtColorBrightnesScale(color, 0.32f), 2.0f);
		ImControlBase::ExtDrawLine(ImVec2(0.0f, WindowCenter.y), ImVec2(size, WindowCenter.y), ImControlBase::ExtColorBrightnesScale(color, 0.32f), 2.0f);
		ImControlBase::ExtDrawCircleFill(IMVEC2_ADD2(PosWinCurrPosition, WindowCenter), PoswinCircleSize, ImVec4(color.x, color.y, color.z, 0.48f));
		
		if (ImGui::IsMouseDown(0) && IMVEC2_DISTANCE(WindowMousePos, PosWinCurrPosition) < PoswinCircleSize && PosWinCurrFocus) {
			PosWinCurrFocus = true;

			index.x -= ImGui::GetIO().MouseDelta.x * Proportion.x;
			index.y -= ImGui::GetIO().MouseDelta.y * Proportion.y;
		}
		else
			PosWinCurrFocus = ImGui::IsMouseClicked(ImGuiMouseButton_Left);

		PosWinCurrPosition.x = LIMIT_CLAMP(PosWinCurrPosition.x, LimitMin.x, WindowCenter.x);
		PosWinCurrPosition.y = LIMIT_CLAMP(PosWinCurrPosition.y, LimitMin.y, WindowCenter.y);
	}
	ImGui::EndChild();
}

void CoordSystemEditorWindow::DrawEditorWindow(
	uint32_t                             unqiue_id,
	const char*                          name,
	std::function<void(CoordSystemInfo)> draw,
	ImTextureID                          poswin_image,
	bool                                 fixed_window,
	float                                speed,
	const ImVec2&                        coord_size,
	const ImVec2&						 coord_winsize,
	bool*                                p_open,
	ImGuiWindowFlags                     flags
) {
	ImVec2 CoordXLimit(-coord_size.x * 0.5f, coord_size.x * 0.5f);
	ImVec2 CoordYLimit(-coord_size.y * 0.5f, coord_size.y * 0.5f);

	ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_None;
	if (fixed_window)
		WindowFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
	ImGui::PushID(unqiue_id);

	// frame_background color.
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImControlBase::ExtColorBrightnesScale(EditorColorSystem, 0.58f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImControlBase::ExtColorBrightnesScale(EditorColorSystem, 0.58f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImControlBase::ExtColorBrightnesScale(EditorColorSystem, 0.58f));
	// slider_block color.
	ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImControlBase::ExtColorBrightnesScale(EditorColorSystem, 0.16f));
	ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImControlBase::ExtColorBrightnesScale(EditorColorSystem, 0.16f));
	ImGui::PushStyleColor(ImGuiCol_Text, ImControlBase::ExtColorBrightnesScale(EditorColorSystem, 0.2f));
	// button color.
	ImGui::PushStyleColor(ImGuiCol_Button, ImControlBase::ExtColorBrightnesScale(EditorColorSystem, 0.58f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImControlBase::ExtColorBrightnesScale(EditorColorSystem, 0.2f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, EditorColorSystem);

	ImGui::Begin(name, p_open, WindowFlags | flags);
	{
		const float RulerWinWidth = 20.0f;
		const ImVec4 RulerCursorColor = ImVec4(0.0f, 1.0f, 1.0f, 1.0f);
		
		ImVec2 GridScaleLimit = {};
		if (CoordXLimit.y - CoordXLimit.x >= CoordYLimit.y - CoordYLimit.x)
			GridScaleLimit = ImVec2(coord_winsize.x / (CoordXLimit.y - CoordXLimit.x), (CoordXLimit.y - CoordXLimit.x) / coord_winsize.x);
		else
			GridScaleLimit = ImVec2(coord_winsize.x / (CoordYLimit.y - CoordYLimit.x), (CoordYLimit.y - CoordYLimit.x) / coord_winsize.x);

		ImVec2 GridWindowPos = ImVec2(RulerWinWidth + IMGUI_ITEM_SPC * 1.5f, RulerWinWidth + IMGUI_ITEM_SPC * 4.75f);
		// grid window scale.
		if (ImGui::IsMouseHoveringRect(
			ImVec2(IMVEC2_ADD2(ImGui::GetWindowPos(), GridWindowPos)),
			ImVec2(IMVEC2_ADD2(ImGui::GetWindowPos(), IMVEC2_ADD2(GridWindowPos, coord_winsize)))
		))
			GridSizeScale.x += ImGui::GetIO().MouseWheel * 0.12f * GridSizeScale.x;

		GridSizeScale.x = LIMIT_CLAMP(GridSizeScale.x, GridScaleLimit.x, GridScaleLimit.y);
		GridSizeScale.y += (GridSizeScale.x - GridSizeScale.y) * 0.1f * speed;

		ImVec2 ViewXLimit(CoordXLimit.x * GridSizeScale.y, CoordXLimit.y * GridSizeScale.y);
		ImVec2 ViewYLimit(CoordYLimit.x * GridSizeScale.y, CoordYLimit.y * GridSizeScale.y);

		// mouse move coord.
		if (ImGui::IsMouseDown(0) && ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows) && GridWinFocus &&
			ImGui::IsMouseHoveringRect(
				ImVec2(IMVEC2_ADD2(ImGui::GetWindowPos(), GridWindowPos)),
				ImVec2(IMVEC2_ADD2(ImGui::GetWindowPos(), IMVEC2_ADD2(GridWindowPos, coord_winsize)))
			)) {
			GridWinFocus = true;

			GridCenterPosition.x += ImGui::GetIO().MouseDelta.x;
			GridCenterPosition.y += ImGui::GetIO().MouseDelta.y;
		}
		else
			GridWinFocus = ImGui::IsMouseClicked(ImGuiMouseButton_Left);

		GridCenterPosition.x = LIMIT_CLAMP(GridCenterPosition.x, ViewXLimit.x, ViewXLimit.y);
		GridCenterPosition.y = LIMIT_CLAMP(GridCenterPosition.y, ViewYLimit.x, ViewYLimit.y);

		GridCenterPositionSmooth.x += (GridCenterPosition.x - GridCenterPositionSmooth.x) * 0.1f * speed;
		GridCenterPositionSmooth.y += (GridCenterPosition.y - GridCenterPositionSmooth.y) * 0.1f * speed;

		ImVec2 WindowMousePos = {};
		ImGui::SetCursorPos(GridWindowPos);
		ImGui::BeginChild("@COORDWIN", coord_winsize, true, ImGuiWindowFlags_NoScrollbar);
		{
			// "2.0f":子窗口边框偏移修正.
			WindowMousePos = ImVec2(ImGui::GetMousePos().x - ImGui::GetWindowPos().x + 2.0f, ImGui::GetMousePos().y - ImGui::GetWindowPos().y + 2.0f);
			WindowMousePos.x = LIMIT_CLAMP(WindowMousePos.x, 0.0f, coord_winsize.x);
			WindowMousePos.y = LIMIT_CLAMP(WindowMousePos.y, 0.0f, coord_winsize.y);
			
			// mouse virtual coord.
			GridMousePosition.x = (WindowMousePos.x - GridCenterPosition.x - coord_winsize.x * 0.5f) / GridSizeScale.y;
			GridMousePosition.y = (GridCenterPosition.y - WindowMousePos.y + coord_winsize.y * 0.5f) / GridSizeScale.y;

			EditorCoordInfo = CoordSystemInfo(
				ImVec2(GridCenterPositionSmooth.x + coord_winsize.x * 0.5f, GridCenterPositionSmooth.y + coord_winsize.y * 0.5f),
				WindowMousePos,
				EditorSeleBoxVirPoints[0], 
				EditorSeleBoxVirPoints[1], 
				GridSizeScale.y
			);

			DrawGrid(CoordXLimit, CoordYLimit, ImControlBase::ExtColorBrightnesScale(EditorColorSystem, 0.64f), GridCenterPositionSmooth, coord_winsize, GridSizeScale.y);
			// grid_draw function.
			draw(EditorCoordInfo);
			// mouse right click. [selection_box]
			if (ImGui::IsMouseDown(1) && ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows)) {
				DrawCoordinateLines(RulerCursorColor, WindowMousePos, coord_winsize);

				if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
					EditorSeleBoxWinPoints[0] = WindowMousePos;
					EditorSeleBoxVirPoints[0] = GridMousePosition;
				}
				EditorSeleBoxWinPoints[1] = WindowMousePos;
				EditorSeleBoxVirPoints[1] = GridMousePosition;

				ImVec2 DrawBoxMin = EXCHPOINTS_MIN(EditorSeleBoxWinPoints[0], EditorSeleBoxWinPoints[1]);
				ImVec2 DrawBoxMax = EXCHPOINTS_MAX(EditorSeleBoxWinPoints[0], EditorSeleBoxWinPoints[1]);

				ImVec2 CalcBoxMin = EXCHPOINTS_MIN(EditorSeleBoxVirPoints[0], EditorSeleBoxVirPoints[1]);
				ImVec2 CalcBoxMax = EXCHPOINTS_MAX(EditorSeleBoxVirPoints[0], EditorSeleBoxVirPoints[1]);

				ImVec2 SeleBoxLen = IMVEC2_SUB2(CalcBoxMax, CalcBoxMin);
				// floating window (display size).
				ImGui::BeginTooltip();
				ImGui::Text("Size:%0.2f", SeleBoxLen.x * SeleBoxLen.y);
				ImGui::EndTooltip();

				// draw selection box color.
				ImVec4 BoxColor = ImControlBase::ExtColorBrightnesScale(EditorColorSystem, 0.58f);
				// draw selection box.
				ImGui::GetWindowDrawList()->AddRectFilled(
					IMVEC2_ADD2(ImGui::GetWindowPos(), DrawBoxMin),
					IMVEC2_ADD2(ImGui::GetWindowPos(), DrawBoxMax),
					IMVEC4_CVT_COLU32(BoxColor),
					ImGui::GetStyle().FrameRounding,
					NULL
				);
				
			}
			else {
				EditorSeleBoxVirPoints[0] = ImVec2(0.0f, 0.0f);
				EditorSeleBoxVirPoints[1] = ImVec2(0.0f, 0.0f);
			}
		}
		ImGui::EndChild();

		ImVec2 RulerDrawPos(GridCenterPositionSmooth.x + coord_winsize.x * 0.5f, GridCenterPositionSmooth.y + coord_winsize.y * 0.5f);
		// x ruler window.
		ImGui::SetCursorPos(ImVec2(RulerWinWidth + IMGUI_ITEM_SPC * 1.5f, IMGUI_ITEM_SPC * 4.25f));
		ImGui::BeginChild("@COORDX", ImVec2(coord_winsize.x, RulerWinWidth));
		{
			ImControlBase::ExtDrawRectangleFill(
				ImVec2(0.0f, 0.0f), 
				ImVec2(coord_winsize.x, RulerWinWidth), 
				ImControlBase::ExtColorBrightnesScale(EditorColorSystem, 0.58f)
			);
			DrawCoordinateXRuler(CoordYLimit, EditorColorSystem, 15.0f, RulerDrawPos.x, GridSizeScale.y);
			// mouse position_x cursor.
			ImControlBase::ExtDrawLine(ImVec2(WindowMousePos.x, 0.0f), ImVec2(WindowMousePos.x, RulerWinWidth), RulerCursorColor, 2.0f);
		}
		ImGui::EndChild();

		// y ruler window.
		ImGui::SetCursorPos(ImVec2(IMGUI_ITEM_SPC, RulerWinWidth + IMGUI_ITEM_SPC * 4.75f));
		ImGui::BeginChild("@COORDY", ImVec2(RulerWinWidth, coord_winsize.y));
		{
			ImControlBase::ExtDrawRectangleFill(
				ImVec2(0.0f, 0.0f),
				ImVec2(RulerWinWidth, coord_winsize.y),
				ImControlBase::ExtColorBrightnesScale(EditorColorSystem, 0.58f)
			);
			DrawCoordinateYRuler(CoordYLimit, EditorColorSystem, 15.0f, RulerDrawPos.y, GridSizeScale.y);
			// mouse position_y cursor.
			ImControlBase::ExtDrawLine(ImVec2(0.0f, WindowMousePos.y), ImVec2(RulerWinWidth, WindowMousePos.y), RulerCursorColor, 2.0f);
		}
		ImGui::EndChild();

		ImVec2 ToolbarSize(240.0f, coord_winsize.y + RulerWinWidth + IMGUI_ITEM_SPC * 0.5f);
		// editor info panel.
		ImGui::SetCursorPos(ImVec2(RulerWinWidth + IMGUI_ITEM_SPC * 2.5f + coord_winsize.x, IMGUI_ITEM_SPC * 4.25f));
		ImGui::BeginChild("@INFOWIN", ToolbarSize);
		{
			ImControlBase::ExtDrawRectangleFill(ImVec2(0.0f, 0.0f), ToolbarSize, ImControlBase::ExtColorBrightnesScale(EditorColorSystem, 0.64f));

			ImGui::SetNextItemWidth(ToolbarSize.x - ImGui::CalcTextSize("Grid").x - IMGUI_ITEM_SPC);
			ImGui::InputFloat2("GRID", &GridCenterPosition.x, "%.0f");

			ImGui::Spacing();
			if (ImGui::Button("RE", ImVec2(32.0f, 0.0f)))
				GridSizeScale.x = 1.0f;
			ImGui::SameLine();
			ImGui::SetNextItemWidth(ToolbarSize.x - IMGUI_ITEM_SPC - 32.0f);
			ImGui::SliderFloat("##SCALE", &GridSizeScale.x, GridScaleLimit.x, GridScaleLimit.y, "%.2f");
			
			ImGui::Spacing();
			PositioningWindow(GridCenterPosition, CoordXLimit, CoordYLimit, EditorColorSystem, poswin_image, ToolbarSize.x, GridSizeScale.y);
			
			ImGui::Spacing();
			ImGui::SetNextItemWidth(ToolbarSize.x);
			ImGui::ColorEdit4("##EDITCOL", &EditorColorSystem.x);

			ImGui::Spacing();
			ImGui::Indent(4.0f);
			ImGui::Text("[BoxP0] x:%.0f y:%.0f", EditorSeleBoxVirPoints[0].x, EditorSeleBoxVirPoints[0].y);
			ImGui::Text("[BoxP1] x:%.0f y:%.0f", EditorSeleBoxVirPoints[1].x, EditorSeleBoxVirPoints[1].y);
			ImGui::Text("[Coord] x:%.0f y:%.0f", GridMousePosition.x, GridMousePosition.y);
			ImGui::Text("[Mouse] x:%.0f y:%.0f", WindowMousePos.x, WindowMousePos.y);
			ImGui::Unindent(4.0f);

			EditorToolbar();
		}
		ImGui::EndChild();
	}
	ImGui::End();
	ImGui::PopStyleColor(9);
	ImGui::PopID();
}