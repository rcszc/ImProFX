// improfx_control_animaxis. RCSZ. [20231220]
// ImGui: [Window(Begin_End)], Animation Axis Editor, Update: 20240216.

#include "improfx_control.h"

constexpr ImVec4 XYZCOL[3] = { ImVec4(1.0f, 0.2f, 0.2f, 1.0f), ImVec4(0.2f, 1.0f, 0.2f, 1.0f), ImVec4(0.5f, 0.5f, 1.0f, 1.0f) };
constexpr ImVec4 PYRCOL[3] = { ImVec4(1.0f, 0.0f, 1.0f, 1.0f), ImVec4(1.0f, 1.0f, 0.0f, 1.0f), ImVec4(0.0f, 1.0f, 1.0f, 1.0f) };

constexpr const char* XYZTEXT[3] = { "Xaxis:%.2f","Yaxis:%.2f","Zaxis:%.2f" };
constexpr const char* PYRTEXT[3] = { "Pitch:%.2f","Yaw:%.2f","Roll:%.2f" };

constexpr float EditorSpaceLeft  = 160.0f;
constexpr float EditorSpaceBelow = 32.0f;

inline void ButtonModeType(bool& modeflag, const char* fname, const char* tname, const ImVec2& size) {
	if (modeflag) {
		if (ImGui::Button(fname, size))
			modeflag = !modeflag;
	}
	else {
		if (ImGui::Button(tname, size))
			modeflag = !modeflag;
	}
}

namespace IMFXC_WIN {
	void AnimAxisEditorWindow::MouseSetPointValue() {
		if (NodeSetValuePointer && ImGui::IsMouseDown(0)) {
			*NodeSetValuePointer -= ImGui::GetIO().MouseDelta.y * 0.92f;

			// floating window(view value).
			ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.25f, 0.25f, 0.25f, 0.72f));
			ImGui::BeginTooltip();
			ImGui::Text("Value:%.1f", *NodeSetValuePointer);
			ImGui::EndTooltip();
			ImGui::PopStyleColor();
		}
		else
			NodeSetValuePointer = nullptr;
	}

	void AnimAxisEditorWindow::SpacingLine(float space_left, const ImVec4& color) {
		ImGui::Spacing();
		ImControlBase::ExtDrawLine(
			ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() - ImGui::GetScrollY()),
			ImVec2(ImGui::GetCursorPosX() + space_left - IMGUI_ITEM_SPC * 2.0f, ImGui::GetCursorPosY() - ImGui::GetScrollY()),
			color, 2.0f
		);
		ImGui::Spacing();
	}

	void AnimAxisEditorWindow::DrawCubicBezierCurve(
		const ImVec2& point0, const ImVec2& point1, const ImVec4& color, const ImVec2& scale,
		float offset, int sample, float centerh
	) {
		// calculate bessel_curve draw_points.
		ImVec2 DrawPoint0(point0.x * scale.x - offset, centerh - point0.y * scale.y);
		ImVec2 DrawPoint1(point1.x * scale.x - offset, centerh - point1.y * scale.y);

		ImVec2 MidPoint = IMVEC2_MUL1(IMVEC2_ADD2(DrawPoint0, DrawPoint1), 0.5f);
		ImVec2 ControlPointA = IMVEC2_SUB2(MidPoint, ImVec2(0.0f, IMVEC2_SUB2(DrawPoint1, MidPoint).y));
		ImVec2 ControlPointB = IMVEC2_ADD2(MidPoint, ImVec2(0.0f, IMVEC2_SUB2(DrawPoint1, MidPoint).y));

		ImVec2 BeginPointTemp = ImBezierCubicCalc(DrawPoint0, ControlPointA, ControlPointB, DrawPoint1, 0.0f);
		for (int i = 0; i < sample; ++i) {

			ImVec2 DrawPoint = ImBezierCubicCalc(DrawPoint0, ControlPointA, ControlPointB, DrawPoint1, (float)i / (float)sample);
			ImControlBase::ExtDrawLine(BeginPointTemp, DrawPoint, color, EditorScaleLinesWidth * 2.0f);
			BeginPointTemp = DrawPoint;
		}
	}

	void AnimAxisEditorWindow::DrawAnimationPoints(const ImVec2& position, float size, const ImVec4& color, float& value) {
		if (&value == NodeSetValuePointer)
			ImControlBase::ExtDrawCircleFill(position, size, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
		else
			ImControlBase::ExtDrawCircleFill(position, size, color);

		ImGui::SetCursorPos(ImVec2(position.x - size, position.y - size));
		ImGui::InvisibleButton("@SETVALUE", ImVec2(size * 2.0f, size * 2.0f));

		if (ImGui::IsItemHovered() && ImGui::IsMouseDown(0) && !NodeSetValuePointer && !PlayerSetYposFlag)
			NodeSetValuePointer = &value;
	}

	void AnimAxisEditorWindow::DrawPlayerLine(ImVec2& position, float offset, float max, const ImVec4& color, float xscale) {
		float PlayerRectWidth = IMGUI_ITEM_SPC * 3.2f;
		float PlayerLineWidth = 2.2f;

		ImControlBase::ExtDrawLine(
			ImVec2(position.y * xscale - offset - PlayerLineWidth * 0.5f, IMGUI_ITEM_SPC + PlayerRectWidth * 0.5f),
			ImVec2(position.y * xscale - offset - PlayerLineWidth * 0.5f, ImGui::GetWindowHeight() - IMGUI_ITEM_SPC * 4.0f),
			color,
			EditorScaleLinesWidth * 2.2f
		);
		ImControlBase::ExtDrawLine(
			ImVec2(position.y * xscale - offset + IMGUI_ITEM_SPC, ImGui::GetWindowHeight() - IMGUI_ITEM_SPC * 4.0f),
			ImVec2(position.y * xscale - offset - IMGUI_ITEM_SPC, ImGui::GetWindowHeight() - IMGUI_ITEM_SPC * 4.0f),
			color,
			EditorScaleLinesWidth * 2.2f
		);

		ImControlBase::ExtDrawRectangleFill(
			ImVec2(position.y * xscale - offset - PlayerRectWidth * 0.5f, IMGUI_ITEM_SPC),
			ImVec2(PlayerRectWidth, IMGUI_ITEM_SPC * 2.0f),
			color
		);
		ImGui::SetCursorPos(ImVec2(position.y * xscale - offset - PlayerRectWidth * 0.5f, IMGUI_ITEM_SPC));
		ImGui::InvisibleButton("@SETPLAYER", ImVec2(PlayerRectWidth, IMGUI_ITEM_SPC * 2.0f));

		if (ImGui::IsItemHovered() && ImGui::IsMouseDown(0) && !NodeSetValuePointer)
			PlayerSetYposFlag = true;
		if (PlayerSetYposFlag && !ImGui::IsMouseDown(0))
			PlayerSetYposFlag = false;
		if (PlayerSetYposFlag)
			position.x += ImGui::GetIO().MouseDelta.x / xscale;

		position.x = position.x < 0.0f ? 0.0f : position.x;
		position.x = position.x > max ? max : position.x;
	}

	bool AnimAxisEditorWindow::RunGetCubicBezierCurve(const ImVec2& point0, const ImVec2& point1, float& value, float playerpos, float centerh) {
		// calculate bessel_curve resul_value.
		ImVec2 CalcPoint0(point0.x, centerh - point0.y);
		ImVec2 CalcPoint1(point1.x, centerh - point1.y);

		if (CalcPoint0.x < playerpos && CalcPoint1.x > playerpos) {
			// bessel_curve control_points.
			ImVec2 MidPoint = IMVEC2_MUL1(IMVEC2_ADD2(CalcPoint0, CalcPoint1), 0.5f);
			ImVec2 ControlPointA = IMVEC2_SUB2(MidPoint, ImVec2(0.0f, IMVEC2_SUB2(CalcPoint1, MidPoint).y));
			ImVec2 ControlPointB = IMVEC2_ADD2(MidPoint, ImVec2(0.0f, IMVEC2_SUB2(CalcPoint1, MidPoint).y));

			value = ImBezierCubicCalc(CalcPoint0, ControlPointA, ControlPointB, CalcPoint1, float((int)playerpos % 100) / (CalcPoint1.x - CalcPoint0.x)).y;
			return true;
		}
		else {
			value = 0.0f;
			return false;
		}
	}

	bool AnimAxisEditorWindow::PlayerRunSample(AnimGenCoord& CoordParam) {
		CoordParam = PlayerRunCoord;
		return PlayerFlag;
	};

	std::vector<AnimGenCoord> AnimAxisEditorWindow::GenerateBakedBezierCurve() {
		std::vector<AnimGenCoord> BakeGenerate = {};

		for (size_t i = 0; i < AnimDataIndex->size() - 1; ++i) {
			AnimCoordSample DatTempA = (*AnimDataIndex)[i];
			AnimCoordSample DatTempB = (*AnimDataIndex)[i + 1];

			for (uint32_t smp = 0; smp < (*AnimDataIndex)[i].BakeSamplingRate; ++smp) {
				AnimGenCoord CoordTemp = {};

				for (size_t j = 0; j < ANE_COORD_PARAMS; ++j) {
					ImVec2 BezPoint0(DatTempA.TimePosition, DatTempA.AnimSamplePoints[j]);
					ImVec2 BezPoint1(DatTempB.TimePosition, DatTempB.AnimSamplePoints[j]);

					ImVec2 MidPoint = IMVEC2_MUL1(IMVEC2_ADD2(BezPoint0, BezPoint1), 0.5f);
					ImVec2 ControlPointA = IMVEC2_SUB2(MidPoint, ImVec2(0.0f, IMVEC2_SUB2(BezPoint1, MidPoint).y));
					ImVec2 ControlPointB = IMVEC2_ADD2(MidPoint, ImVec2(0.0f, IMVEC2_SUB2(BezPoint1, MidPoint).y));

					ImVec2 DrawPoint = ImBezierCubicCalc(BezPoint0, ControlPointA, ControlPointB, BezPoint1, (float)smp / (float)DatTempA.BakeSamplingRate);
					CoordTemp.AnimGenVector[j] = DrawPoint.x;
				}
				BakeGenerate.push_back(CoordTemp);
			}
		}
		return BakeGenerate;
	}

	void AnimAxisEditorWindow::DrawEditorWindow(
		uint32_t                      unqiue_id,
		const char*					  name,
		float                         track_length,
		float                         player_step,
		std::vector<AnimCoordSample>& sample,
		bool                          fixed_window,
		bool*						  p_open,
		ImGuiWindowFlags              flags
	) {
		AnimDataIndex = &sample;

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
			ImVec2 ChildWindowSize = ImVec2(ImGui::GetWindowSize().x - EditorSpaceLeft, ImGui::GetWindowSize().y - EditorSpaceBelow);

			ImGui::SetCursorPos(ImVec2(EditorSpaceLeft, ImGui::GetWindowSize().y - EditorSpaceBelow - IMGUI_ITEM_SPC * 0.5f));
			ImGui::SetNextItemWidth(ChildWindowSize.x - ImGui::CalcTextSize("TRACK").x - IMGUI_ITEM_SPC * 2.0f);

			TrackWindowXpos.y = track_length * TrackWidthValueScale - ChildWindowSize.x * 0.85f;
			TrackWindowXpos.y = TrackWindowXpos.y < 0.0f ? 0.0f : TrackWindowXpos.y;

			ImGui::SliderFloat("TRACK", &TrackWindowXpos.x, 0.0f, TrackWindowXpos.y, "%0.1f");

			ImGui::SetCursorPos(ImVec2(EditorSpaceLeft, IMGUI_ITEM_SPC * 4.32f));
			// draw animation_track editor window.
			ImGui::BeginChild("@ANIMTRACK", ImVec2(ChildWindowSize.x - IMGUI_ITEM_SPC, ChildWindowSize.y - IMGUI_ITEM_SPC * 5.5f), true);

			ImVec4 LinesColor = ImControlBase::ExtColorBrightnesScale(EditorColorSystem, 0.42f);
			// value_zero base_line.
			float HighCenterPosition = ImGui::GetWindowHeight() * 0.5f;
			TrackXpos += (TrackWindowXpos.x - IMGUI_ITEM_SPC * 4.0f - TrackXpos) * 0.1f;

			for (float i = 0.0f; i <= track_length; i += 50.0f) {
				float LinesXposTemp = i * TrackWidthValueScale - TrackXpos;

				ImControlBase::ExtDrawLine(
					ImVec2(LinesXposTemp, IMGUI_ITEM_SPC),
					ImVec2(LinesXposTemp, ImGui::GetWindowHeight() - IMGUI_ITEM_SPC * 4.0f),
					LinesColor,
					EditorScaleLinesWidth
				);
			}

			for (float i = 0.0f; i <= track_length; i += 100.0f) {
				char TimeTickText[32] = {};
				sprintf_s(TimeTickText, "%.0f", i);

				float LinesXposTemp = i * TrackWidthValueScale - TrackXpos;
				// draw time_tick text.
				ImControlBase::ExtDrawText(
					ImVec2(LinesXposTemp - ImGui::CalcTextSize(TimeTickText).x * 0.5f, ImGui::GetWindowHeight() - IMGUI_ITEM_SPC * 3.5f),
					LinesColor, "%.0f", i
				);

				ImControlBase::ExtDrawLine(
					ImVec2(LinesXposTemp, IMGUI_ITEM_SPC),
					ImVec2(LinesXposTemp, ImGui::GetWindowHeight() - IMGUI_ITEM_SPC * 4.0f),
					LinesColor,
					EditorScaleLinesWidth * 2.0f
				);
			}

			// draw centerline[value: 0.0f].
			ImControlBase::ExtDrawLine(
				ImVec2(0.0f, HighCenterPosition),
				ImVec2(ImGui::GetWindowWidth(), HighCenterPosition),
				LinesColor,
				EditorScaleLinesWidth * 1.8f
			);

			ImVec4 AsixColors[3] = {};
			const char* AsixTexts[3] = {};

			if (EditorModeType) {
				std::copy(std::begin(PYRCOL), std::end(PYRCOL), std::begin(AsixColors));
				std::copy(std::begin(PYRTEXT), std::end(PYRTEXT), std::begin(AsixTexts));
			}
			else {
				std::copy(std::begin(XYZCOL), std::end(XYZCOL), std::begin(AsixColors));
				std::copy(std::begin(XYZTEXT), std::end(XYZTEXT), std::begin(AsixTexts));
			}

			for (size_t i = 0; i < sample.size() - 1; ++i) {
				float SampleCoordIndex[ANE_COORD_PARAMS] = {};
				size_t AnimCount = size_t(sample.size() >= 2);

				for (size_t j = 0; j < 3; ++j) {
					size_t ParamCount = j + (size_t)EditorModeType * 3;

					DrawCubicBezierCurve(
						ImVec2(sample[i].TimePosition, sample[i].AnimSamplePoints[ParamCount]),
						ImVec2(sample[i + AnimCount].TimePosition, sample[i + AnimCount].AnimSamplePoints[ParamCount]),
						AsixColors[j],
						ImVec2(TrackWidthValueScale, TrackHeightValueScale),
						TrackXpos,
						sample[i].PlayerSamplingRate,
						HighCenterPosition
					);
					DrawAnimationPoints(
						ImVec2(sample[i].TimePosition * TrackWidthValueScale - TrackXpos, HighCenterPosition - sample[i].AnimSamplePoints[ParamCount] * TrackHeightValueScale),
						4.2f,
						AsixColors[j],
						sample[i].AnimSamplePoints[ParamCount]
					);
				}
				for (size_t j = 0; j < 6; ++j) {
					if (RunGetCubicBezierCurve(
						ImVec2(sample[i].TimePosition, sample[i].AnimSamplePoints[j]),
						ImVec2(sample[i + AnimCount].TimePosition, sample[i + AnimCount].AnimSamplePoints[j]),
						SampleCoordIndex[j],
						PlayerLineXpos.y,
						HighCenterPosition
					))
						PlayerRunCoord.AnimGenVector[j] = HighCenterPosition - SampleCoordIndex[j];
				}
			}
			size_t LAST = sample.size() - 1;
			for (size_t j = 0; j < 3; ++j) {
				size_t ParamCount = j + (size_t)EditorModeType * 3;

				DrawAnimationPoints(
					ImVec2(sample[LAST].TimePosition * TrackWidthValueScale - TrackXpos, HighCenterPosition - sample[LAST].AnimSamplePoints[ParamCount] * TrackHeightValueScale),
					4.2f,
					AsixColors[j],
					sample[LAST].AnimSamplePoints[ParamCount]
				);
			}

			MouseSetPointValue();
			DrawPlayerLine(PlayerLineXpos, TrackXpos, track_length * TrackWidthValueScale, EditorColorPlayer, TrackWidthValueScale);

			ImGui::EndChild();

			PlayerLineXpos.y += (PlayerLineXpos.x - PlayerLineXpos.y) * 0.12f;
			PlayerLineXpos.y = abs(PlayerLineXpos.x - PlayerLineXpos.y) < 0.01f ? PlayerLineXpos.x : PlayerLineXpos.y;

			ImGui::SetCursorPos(ImVec2(IMGUI_ITEM_SPC, IMGUI_ITEM_SPC * 4.32f));
			ImGui::Text("TICK: %.1f", PlayerLineXpos.y);
			ImGui::Spacing();

			float ButtonWidth = (EditorSpaceLeft - IMGUI_ITEM_SPC * 3.0f) * 0.5f;
			if (ImGui::Button("Begin", ImVec2(ButtonWidth, 0.0f))) {
				PlayerLineXpos.x = 0.0f;
				PlayerLineXpos.y = 0.0f;
			}
			ImGui::SameLine();
			if (ImGui::Button("End", ImVec2(ButtonWidth, 0.0f))) {
				PlayerLineXpos.x = track_length;
				PlayerLineXpos.y = track_length;
			}
			ImGui::Spacing();

			float SpaceLeftItem = EditorSpaceLeft - IMGUI_ITEM_SPC * 2.0f;
			ButtonModeType(PlayerFlag, "STOP", "PLAY", ImVec2(SpaceLeftItem, 0.0f));
			if (PlayerFlag)
				PlayerLineXpos.x += player_step * TrackWidthValueScale;

			ImGui::Spacing();
			ButtonModeType(EditorModeType, "X.Y.Z", "P.Y.R", ImVec2(SpaceLeftItem, 0.0f));

			ImGui::Spacing();
			if (ImGui::Button("RH", ImVec2(32.0f, 0.0f)))
				TrackHeightValueScale = 1.0f;
			ImGui::SameLine();
			ImGui::SetNextItemWidth(SpaceLeftItem - 32.0f - IMGUI_ITEM_SPC);
			ImGui::SliderFloat("##HIGHSCALE", &TrackHeightValueScale, 0.02f, 2.0f, "%.2f");

			ImGui::Spacing();
			if (ImGui::Button("RW", ImVec2(32.0f, 0.0f)))
				TrackWidthValueScale = 1.0f;
			ImGui::SameLine();
			ImGui::SetNextItemWidth(SpaceLeftItem - 32.0f - IMGUI_ITEM_SPC);
			ImGui::SliderFloat("##WIDTHSCALE", &TrackWidthValueScale, 0.5f, 2.0f, "%.2f");

			ImGui::Spacing();
			ImGui::SetNextItemWidth(SpaceLeftItem);
			ImGui::InputFloat("##SETPOS", &PlayerLineXpos.x, 0.0f, 0.0f, "%.1f");

			SpacingLine(EditorSpaceLeft, LinesColor);
			{
				for (size_t i = 0; i < 3; ++i)
					ImGui::TextColored(AsixColors[i], AsixTexts[i], PlayerRunCoord.AnimGenVector[i + (size_t)EditorModeType * 3]);
			}
			SpacingLine(EditorSpaceLeft, LinesColor);

			ImGui::Text("Animations:%u", sample.size());
		}
		ImGui::End();
		ImGui::PopStyleColor(9);
		ImGui::PopID();
	}
}