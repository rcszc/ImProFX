// framework_render. [2023_10_12] RCSZ.

#ifndef _FRAMEWORK_RENDER_HPP
#define _FRAMEWORK_RENDER_HPP
#include <fstream>
#include <filesystem>
#include <document.h> // RapidJSON

#include "../improfx_shader/framework_shader.h"
#include "../improfx_anim/framework_animation.hpp"
#include "framework_window.h"
#include "framework_imgui.h"

#define SYSTEM_MODULE_LABLE_render std::string("[render]: ")

namespace ImFxFind {
	/*
	* 查询渲染项(item)
	* @param  input (DataFxRender), text (Key)
	* @return FxRenderItem (独立shader渲染项)
	*/
	FxRenderItem FindRenderItem(std::unordered_map<std::string, FxRenderItem>& input, const char* text);
	/*
	* 查询渲染项FBO附着纹理句柄
	* @param  input (DataFxRender), text (Key)
	* @return uint32_t (纹理句柄)
	*/
	uint32_t FindRenderItemFBO(std::unordered_map<std::string, FxRenderItem>& input, const char* text);
	/*
	* 查询设置渲染项参数(shader非系统uniform[float])
	* @param  input (DataFxRender), text (Key), ufn (ParamKey), value
	* @return void
	*/
	void FindSetRenderItemParam(std::unordered_map<std::string, FxRenderItem>& input, const char* text, const char* ufn, float value);
	/*
	* 查询设置渲染项标志(shader程序运行条件)
	* @param  input (DataFxRender), text (Key), flag
	* @return void
	*/
	void FindSetRenderItemFlag(std::unordered_map<std::string, FxRenderItem>& input, const char* text, bool flag);
	/*
	* 查询源纹理图片句柄(与渲染项无关)
	* @param  input (DataRawTexture), text (Key)
	* @return uint32_t (纹理句柄)
	*/
	uint32_t FindRawTextureImg(std::unordered_map<std::string, FStextureHandle>& input, const char* text);
}

namespace CoreModuleRender {

	class FrameworkRender :public
		INTERFACE_PROFX_START,
		CoreModuleWindow::RenderWindow,
		CoreModuleImGui::RenderImGui
	{
	protected:
		// imgui component animation.
		std::unordered_map<std::string, ImGuiProAnim::ImAnimationBase*> DataCompAnim = {};
		// shader fx render pool. hash.key = str.
		std::unordered_map<std::string, FxRenderItem> DataFxRender = {};
		// raw texture(no shader). tex => imgui.
		std::unordered_map<std::string, FStextureHandle> DataRawTexture = {};
		
		// x:width, y:height. 
		Vector2T<float> WindowSize = {};

		// max number texture sampling unit.
		uint32_t MaxTextureSample = NULL;

		// time tick smooth value.
		float TimerSmoothValue[2] = { 1.0f,1.0f };  // 0:value, 1:smooth_value
		float TimerSmoothCount = 100.0f;            // start fps 100.0f
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