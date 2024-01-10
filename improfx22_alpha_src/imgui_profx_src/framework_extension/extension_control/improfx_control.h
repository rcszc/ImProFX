// improfx_control. RCSZ.
// ImProFX Framework => ImGui ExtControl
// Update: 2023.12.26

#ifndef _IMPROFX_CONTROL_H
#define _IMPROFX_CONTROL_H
#include <functional>
#include <string>
#include <vector>
#include <deque>
#include <cstdio>

#include "improfx_control_base.h"

// ######################################## SmoothMenuChildWindow ########################################
// [ChildWindow]: 动画纵向菜单. 2023_12_16 RCSZ.
class SmoothMenuChildWindow {
protected:
	float TextDrawHeight = 0.0f;

	ImVec2 MenuBufferYposType  = {};
	ImVec2 MenuBufferWidthType = {};

	ImVec2 MenuBufferYposItem  = {};
	ImVec2 MenuBufferWidthItem = {};

	void DrawMenuTypeRect(float rect_height, const ImVec4& color);
	void DrawMenuItemRect(float rect_height, const ImVec4& color);
	void MenuInterCalc(ImVec2& posy_calc, ImVec2& width_calc, float speed);

public:
	void DrawMenuWindow(
		const char*                     name,
		const std::vector<std::string>& items,
		uint32_t&                       count,
		const ImVec4&                   color     = ImVec4(0.55f, 0.12f, 1.0f, 0.72f),
		const ImVec2&                   size      = ImVec2(256.0f, 384.0f),
		float                           speed     = 1.0f,      
		float                           textscale = 1.2f
	);
};

// ######################################## AnimNodesEditorWindow ########################################
#define ANE_COORD_PARAM 6 
// animation points coordinates.
struct AnimCoordSample {
	float AnimSamplePoints[ANE_COORD_PARAM];
	float TimePosition;

	uint32_t PlayerSamplingRate;
	uint32_t BakeSamplingRate;

	AnimCoordSample() : AnimSamplePoints{}, TimePosition(0.0f), PlayerSamplingRate(128), BakeSamplingRate(256) {}
	AnimCoordSample(float x, float y, float z, float pt, float yw, float rl, float t, uint32_t ps = 128, uint32_t gs = 256) :
		AnimSamplePoints{ x, y, z, pt, yw, rl }, TimePosition(t), PlayerSamplingRate(ps), BakeSamplingRate(gs)
	{}
};

struct AnimGenCoord {
	float AnimGenVector[ANE_COORD_PARAM];
};

// [Window]: 动画(节点)编辑器. 2023_12_19 RCSZ.
class AnimNodesEditorWindow {
protected:
	ImVec4 EditorColorPlayer = ImVec4(0.0f, 0.72f, 0.72f, 1.0f);

	float EditorScaleLinesWidth = 1.0f;
	bool  EditorModeType        = false; // false:[x.y.z], true:[pitch.yaw.roll]

	ImVec2 TrackWindowXpos = {};   // x:pos, y:max.
	float  TrackXpos       = 0.0f; // tick inter_smooth.

	float* NodeSetValuePointer = nullptr;

	ImVec2 PlayerLineXpos    = {}; // x:pos, y:smooth
	bool   PlayerSetYposFlag = false;
	bool   PlayerPlayFlag    = false;

	AnimGenCoord PlayerRunCoord = {};
	std::vector<AnimCoordSample>* AnimDataIndex = nullptr;

	void MouseSetPointValue();
	void SpacingLine(float space_left, const ImVec4& color);

	void DrawCubicBezierCurve(
		const ImVec2& point0, const ImVec2& point1, const ImVec4& color, const ImVec2& scale,
		float offset, int sample, float centerh
	);
	void DrawAnimationPoints(const ImVec2& position, float size, const ImVec4& color, float& value);
	void DrawPlayerLine(ImVec2& position, float offset, float max, const ImVec4& color, float xscale);

	bool RunGetCubicBezierCurve(const ImVec2& point0, const ImVec2& point1, float& value, float playerpos, float centerh);

public:
	bool PlayerRunSample(AnimGenCoord& CoordParam);
	std::vector<AnimGenCoord> GenerateBakedBezierCurve();

	void DrawEditorWindow(
		uint32_t                      unqiue_id,
		const char*                   name,
		float                         track_length,
		float                         player_step,
		std::vector<AnimCoordSample>& sample,
		bool                          fixed_window = false,
		bool*                         p_open       = (bool*)0,
		ImGuiWindowFlags              flags        = 0
	);

	float TrackWidthValueScale = 1.0f;
	float TrackHeightValueScale = 1.0f;

	ImVec4 EditorColorSystem = ImVec4(0.85f, 0.85f, 0.85f, 1.0f);
};

// ######################################## CoordSystemEditorWindow ########################################
// editor coordinate information.
struct CoordSystemInfo {
	ImVec2 CenterPosition;
	ImVec2 MouseCoordPos;

	ImVec2 SelectionBoxPoint0;
	ImVec2 SelectionBoxPoint1;

	float ScaleCoord;

	CoordSystemInfo() : CenterPosition({}), MouseCoordPos({}), SelectionBoxPoint0({}), SelectionBoxPoint1({}), ScaleCoord(1.0f) {}
	CoordSystemInfo(const ImVec2& cen, const ImVec2& pos, const ImVec2& bxp0, const ImVec2& bxp1, float scale):
		CenterPosition(cen), MouseCoordPos(pos), SelectionBoxPoint0(bxp0), SelectionBoxPoint1(bxp1), ScaleCoord(scale)
	{}
};

// [Window]: 坐标系编辑器. 2023_12_25 RCSZ.
class CoordSystemEditorWindow {
protected:
	CoordSystemInfo EditorCoordInfo = {};

	ImVec2 GridCenterPosition       = {};
	ImVec2 GridCenterPositionSmooth = {};

	ImVec2 GridSizeScale = ImVec2(1.0f, 1.0f); // x:scale, y:smooth
	ImVec2 GridMousePosition = {};

	ImVec2 EditorSeleBoxWinPoints[2] = {};
	ImVec2 EditorSeleBoxVirPoints[2] = {};

	ImVec2 PosWinCurrPosition = {};

	bool GridWinFocus    = false;
	bool PosWinCurrFocus = false;

	void DrawCoordinateXRuler(const ImVec2& limit, const ImVec4& color, float length, float center, float scale, float ruler = 20.0f);
	void DrawCoordinateYRuler(const ImVec2& limit, const ImVec4& color, float length, float center, float scale, float ruler = 20.0f);
	void DrawCoordinateLines(const ImVec4& color, const ImVec2& mouse, const ImVec2& win_size);

	void DrawGrid(
		const ImVec2& limitx, const ImVec2& limity, const ImVec4& color, const ImVec2& pos, const ImVec2& win_size,
		float scale, float ruler = 20.0f
	);
	// child window.
	void PositioningWindow(
		ImVec2& index, const ImVec2& limitx, const ImVec2& limity, const ImVec4& color, ImTextureID texture,
		float size, float scale
	);

public:
	void DrawEditorWindow(
		uint32_t							 unqiue_id,
		const char*							 name,
		std::function<void(CoordSystemInfo)> draw,
		ImTextureID                          poswin_image  = nullptr,
		bool								 fixed_window  = true,
		float								 speed         = 1.0f,
		const ImVec2&                        coord_size    = ImVec2(1200.0f, 1200.0f),
		const ImVec2&						 coord_winsize = ImVec2(640.0f, 640.0f),
		bool*                                p_open        = (bool*)0,
		ImGuiWindowFlags                     flags         = 0
	);

	std::function<void()> EditorToolbar = []() {};
	ImVec4 EditorColorSystem = ImVec4(0.85f, 0.85f, 0.85f, 1.0f);
};

#endif