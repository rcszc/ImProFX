// improfx_control_base. RCSZ.

#ifndef _IMPROFX_CONTROL_BASE_HPP
#define _IMPROFX_CONTROL_BASE_HPP

#include "imgui_glfw/imgui.h"
#include "imgui_glfw/imgui_internal.h"

#define IMFXC_PI 3.1415927f
#define IMFXC_DEGTORAD(deg) ((deg) * IMFXC_PI / 180.0f)

#define IMVEC2_ADD2(p1, p2) ImVec2((p1).x + (p2).x, (p1).y + (p2).y)
#define IMVEC2_SUB2(p1, p2) ImVec2((p1).x - (p2).x, (p1).y - (p2).y)
#define IMVEC2_MUL1(p, v)   ImVec2((p).x * (v), (p).y * (v))
#define IMVEC2_MUL2(p1, p2) ImVec2((p1).x * (p2).x, (p1).y * (p2).y)

#define IMVEC2_DISTANCE(pos1, pos2) std::sqrt((pos1.x - pos2.x) * (pos1.x - pos2.x) + (pos1.y - pos2.y) * (pos1.y - pos2.y))

#define IMFP32_CVT_COLU32(R, G, B, A) IM_COL32(ImU32(R * 255.0f), ImU32(G * 255.0f), ImU32(B * 255.0f), ImU32(A * 255.0f))
#define IMVEC4_CVT_COLU32(COLOR)      IM_COL32(ImU32((COLOR).x * 255.0f), ImU32((COLOR).y * 255.0f), ImU32((COLOR).z * 255.0f), ImU32((COLOR).w * 255.0f))

#define IMGUI_ITEM_SPC ImGui::GetStyle().ItemSpacing.x
namespace ImControlBase {

	ImVec4 ExtColorBrightnesScale(const ImVec4& color, float value);
	float ExtItemCenteredCalc(const float& width);

	void ExtDrawLine(const ImVec2& point0, const ImVec2& point1, const ImVec4& color, float linewidth);
	void ExtDrawText(const ImVec2& position, const ImVec4& color, const char* text, ...);

	void ExtDrawRectangle(const ImVec2& position, const ImVec2& size, const ImVec4& color, float linewidth);
	void ExtDrawRectangleFill(const ImVec2& position, const ImVec2& size, const ImVec4& color);
	void ExtDrawCircleFill(const ImVec2& position, float size, const ImVec4& color);
	void ExtDrawTriangleFill(const ImVec2& position, const ImVec2& offset1, const ImVec2& offset2, const ImVec4& color);
}

#endif