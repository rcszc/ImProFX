// ext_imguicontrol. ImGui第三方扩展框架封装.

#ifndef _EXT_IMGUICONTROL_HPP
#define _EXT_IMGUICONTROL_HPP
#include "framework_extension_tools.hpp"

#ifdef ENABLE_IMGUI_EXTEND
#include "../guiimgui_extend/imgui_knobs/imgui-knobs.h"
#include "../guiimgui_extend/imgui_memory_editor/imgui_memory_editor.h"

// knobs:         https://github.com/altschuler/imgui-knobs
// memory_editor: https://github.com/ocornut/imgui_club

enum ImExtKnobsType {
	KnobInt   = 1 << 1,
	KnobFloat = 1 << 2
};
class ImExtKnobs {
protected:
	int   ValueInt   = NULL;
	float ValueFloat = 0.0f;
public:
	int* GetIntPointer() { return &ValueInt; }
	float* GetFloatPointer() { return &ValueFloat; }

	bool DrawKnobs(
		ImExtKnobsType   type,
		const char*      name,
		const ImVec2&    limit, // x:min,y:max
		float            speed   = 0.0f,
		const char*      format  = "%.2f",
		ImGuiKnobVariant variant = 1,
		float            size    = 0.0f,
		ImGuiKnobFlags   flags   = 0,
		int              steps   = 10
	) {
		switch (type) {
		case(KnobFloat): { return ImGuiKnobs::Knob(name, &ValueFloat, limit.x, limit.y, speed, format, variant, size, flags, steps); }
		case(KnobInt): { return ImGuiKnobs::KnobInt(name, &ValueInt, (int)limit.x, (int)limit.y, speed, format, variant, size, flags, steps); }
		default: return false;
		}
	}
};

template<typename T>
class ImExtMemoryEditor {
protected:
	MemoryEditor EditorExecution = {};
	std::vector<T> MemoryData = {};
public:
	ImExtMemoryEditor() : MemoryData(std::vector<T>(128)) {}
	// std::vector pointer.
	std::vector<T>* GetMemoryPointer() { return &MemoryData; }

	void DrawMemoryEditor(const char* name) {
		EditorExecution.DrawWindow(name, MemoryData.data(), MemoryData.size() * sizeof(T));
	}
};

// ExtImGui 全局控件间距(单位量).
#define EXT_IMGUI_ITEM_SPC ImGui::GetStyle().ItemSpacing.x
namespace ImExtAnim {
	namespace ImExtSystem {

		inline ImVec4 ExtSubColorVec(const ImVec4& color, float value) {
			return ImVec4(
				color.x - value < 0.0f ? 0.0f : color.x - value,
				color.y - value < 0.0f ? 0.0f : color.y - value,
				color.z - value < 0.0f ? 0.0f : color.z - value,
				color.w - value < 0.0f ? 0.0f : color.w - value
			);
		}

		inline float ExtItemCenteredCalc(const float& width) {
			return ImGui::GetWindowSize().x / 2.0f - width / 2.0f - IMGUI_ITEM_SPC;
		}

		inline void ExtDrawRectangleFill(const ImVec2& position, const ImVec2& size, const ImVec4& color) {
			// draw fill_rectangle.
			ImGui::GetWindowDrawList()->AddRectFilled(
				ImVec2(ImGui::GetWindowPos().x + position.x, ImGui::GetWindowPos().y + position.y),
				ImVec2(ImGui::GetWindowPos().x + position.x + size.x, ImGui::GetWindowPos().y + position.y + size.y),
				IM_COL32(ImU32(color.x * 255.0f), ImU32(color.y * 255.0f), ImU32(color.z * 255.0f), ImU32(color.w * 255.0f)),
				ImGui::GetStyle().FrameRounding,
				NULL
			);
		}
	}

	// 动画菜单子窗口. 2023_12_16 RCSZ.
	class AnimMenuChildWindow {
	protected:
		float TextDrawHeight = 0.0f;

		ImVec2 AnimBufferPosyType  = {};
		ImVec2 AnimBufferWidthType = {};

		ImVec2 AnimBufferPosyItem = {};
		ImVec2 AnimBufferWidthItem = {};

		void DrawMenuTypeRect(float rect_height, const ImVec4& color) {
			// fill selection box.
			ImExtSystem::ExtDrawRectangleFill(
				ImVec2(ImExtSystem::ExtItemCenteredCalc(AnimBufferWidthType.y), AnimBufferPosyType.y),
				ImVec2(AnimBufferWidthType.y, rect_height + 2.0f),
				color
			);
			ImGui::SetCursorPos(ImVec2(0.0f, 0.0f));
		}

		void DrawMenuItemRect(float rect_height, const ImVec4& color) {
			// fill item box.
			ImExtSystem::ExtDrawRectangleFill(
				ImVec2(ImExtSystem::ExtItemCenteredCalc(AnimBufferWidthItem.y), AnimBufferPosyItem.y),
				ImVec2(AnimBufferWidthItem.y, rect_height + 2.0f),
				color
			);
			// fill item_location box.
			ImExtSystem::ExtDrawRectangleFill(ImVec2(ImGui::GetWindowSize().x - 12.0f, AnimBufferPosyItem.y), ImVec2(8.0f, rect_height + 2.0f), color);
			ImGui::SetCursorPos(ImVec2(0.0f, 0.0f));
		}

		void MenuLerpCalc(ImVec2& posy_calc, ImVec2& width_calc) {
			// 线性插值: result = y + (x − y) * t.
			posy_calc.y += (posy_calc.x - posy_calc.y) * 0.05f;
			width_calc.y += (width_calc.x - width_calc.y) * 0.05f;
		}

	public:
		void DrawMenuWindow(
			const char*                     name,
			const std::vector<std::string>& items,
			uint32_t&                       count,
			const ImVec4&                   color     = ImVec4(0.55f, 0.12f, 1.0f, 0.72f),
			const ImVec2&                   size      = ImVec2(256.0f, 384.0f),
			float                           textscale = 1.2f
		) {
			ImGui::BeginChild(name, size, true);
			ImGui::SetWindowFontScale(textscale);

			DrawMenuTypeRect(TextDrawHeight, color);
			ImVec4 ImGuiTextColor = ImGui::GetStyleColorVec4(ImGuiCol_Text);
			DrawMenuItemRect(TextDrawHeight, ImExtSystem::ExtSubColorVec(ImGuiTextColor, 0.25f));

			for (size_t i = 0; i < items.size(); ++i) {

				ImVec2 ItemTextSize = ImGui::CalcTextSize(items[i].c_str());
				float DrawPosyTemp = ImGui::GetCursorPosY();

				ImGui::SetCursorPosX(ImExtSystem::ExtItemCenteredCalc(ItemTextSize.x));
				ImGui::Text(items[i].c_str());

				if (ImGui::IsItemHovered()) {
					AnimBufferPosyType.x = DrawPosyTemp;
					AnimBufferWidthType.x = ItemTextSize.x + EXT_IMGUI_ITEM_SPC * 2.0f;

					if (ImGui::GetMouseClickedCount(0))
						count = (uint32_t)i;
				}
				if (count == (uint32_t)i) {
					AnimBufferPosyItem.x = DrawPosyTemp;
					AnimBufferWidthItem.x = ItemTextSize.x + EXT_IMGUI_ITEM_SPC * 2.0f;
				}
				TextDrawHeight = ItemTextSize.y;
			}
			MenuLerpCalc(AnimBufferPosyType, AnimBufferWidthType);
			MenuLerpCalc(AnimBufferPosyItem, AnimBufferWidthItem);

			ImGui::EndChild();
		}
	};
}

#endif
#endif