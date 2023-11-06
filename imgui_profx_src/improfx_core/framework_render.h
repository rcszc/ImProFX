// framework_render. [2023_10_12] RCSZ.

#ifndef _FRAMEWORK_RENDER_HPP
#define _FRAMEWORK_RENDER_HPP
#include <fstream>
#include <filesystem>
#include <any>
#include <document.h> // RapidJSON

#include "../improfx_shader/framework_shader.h"
#include "../improfx_anim/framework_animation.hpp"
#include "../improfx_memory/framework_memory.h"
#include "framework_window.h"
#include "framework_imgui.h"

#define SYSTEM_MODULE_LABLE_render std::string("[framework_core]: ")

namespace ImFxFind {

	FxRenderItem FindRenderItem(std::unordered_map<std::string, FxRenderItem>& input, const char* text);
	uint32_t     FindRenderItemFBO(std::unordered_map<std::string, FxRenderItem>& input, const char* text);

	void FindSetRenderItemParam(std::unordered_map<std::string, FxRenderItem>& input, const char* text, const char* ufn, float value);
	void FindSetRenderItemFlag(std::unordered_map<std::string, FxRenderItem>& input, const char* text, bool flag);

	uint32_t FindRawTextureImg(std::unordered_map<std::string, FStextureHandle>& input, const char* text);

	void  StaticFlagAlloc(std::unordered_map<std::string, bool>& input, const char* text, bool init = false, bool REPEAT = false);
	void  StaticFlagDelete(std::unordered_map<std::string, bool>& input, const char* text);
	bool* StaticFlagFind(std::unordered_map<std::string, bool>& input, const char* text);
}

namespace CoreModuleRender {

	class FrameworkRender :public INTERFACE_PROFX_START,
		CoreModuleWindow::RenderWindow,
		CoreModuleImGui::RenderImGui
	{
	protected:
		std::unordered_map<std::string, FxRenderItem>                   DataFxRender   = {}; // shader fx render. hash.key = str.
		std::unordered_map<std::string, FStextureHandle>                DataRawTexture = {}; // raw texture. tex => imgui.
		std::unordered_map<std::string, ImGuiProAnim::ImAnimationBase*> DataCompAnim   = {}; // imgui component animation.
		std::unordered_map<std::string, ImDynamicMem::ImMemory*>        DataDynamicMem = {}; // dynamic memory.

		std::unordered_map<std::string, bool> StaticFlag = {}; // boolean flags.
		
		// x:width, y:height.
		Vector2T<float> WindowSize = {};

		// max number texture mapping unit.
		uint32_t MaxNumberTMU = NULL;

		// time tick smooth value.
		float TimerSmoothValue[2] = { 1.0f,1.0f };  // 0:value, 1:smooth_value
		float TimerSmoothCount    = 100.0f;         // start run_speed 100.0f
		std::chrono::steady_clock::time_point SampleTimer = std::chrono::steady_clock::now();

		rapidjson::Document ReadJsonConfig(std::string file);
		
		size_t ShaderConfigLoad(rapidjson::Document config);

		// render tick function.
		void RenderSceneFBO();

		// init: return flag: false:failed, true:success.
		bool LogicInitialization();
		// loop: return flag: false:continue, true:exit.
		bool LogicEventLoop();
		void LogicCloseFree();

		void calculate_smooth_trans(float preset_fps, int64_t sample_time);

	public:
		bool ProfxInit();
		bool ProfxFree();
		int32_t ProfxEventLoop();
	};
}

namespace CoreModuleSystem {

	class ProfxSystemFramework {
	protected:
		CoreModuleRender::FrameworkRender* render_object = nullptr;

	public:
		ProfxSystemFramework(CoreModuleRender::FrameworkRender* OBJ) {
			render_object = OBJ;
		}
		int32_t StartProfx();
	};
}

#endif