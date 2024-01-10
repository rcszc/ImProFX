// ext_imguicontrol. ImGui第三方扩展框架封装.

#ifndef _EXT_IMGUICONTROL_HPP
#define _EXT_IMGUICONTROL_HPP
#include <cstdio>
#include "framework_extension_tools.hpp"

// imgui control.
#include "imgui_glfw/imgui.h"
#include "imgui_glfw/imgui_internal.h"

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

#endif
#endif