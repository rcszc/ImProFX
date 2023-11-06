// framework_render_loop.
#include "framework_render.h"

using namespace std;
using namespace LOGCONS;

namespace ImFxFind {

	FxRenderItem FindRenderItem(unordered_map<string, FxRenderItem>& input, const char* text) {
		return (input.find(text) != input.end()) ? input[text] : FxRenderItem();
	}

	uint32_t FindRenderItemFBO(unordered_map<string, FxRenderItem>& input, const char* text) {
		return ((input.find(text) != input.end()) ? input[text] : FxRenderItem()).FrameBufferTex.texture_hd;
	}

	void FindSetRenderItemParam(unordered_map<string, FxRenderItem>& input, const char* text, const char* ufn, float value) {
		auto it = input.find(text);
		if (it != input.end()) {
			auto ituf = it->second.ParamValueFloat.find(ufn);
			if (ituf != it->second.ParamValueFloat.end())
				ituf->second = value;
		}
	}

	void FindSetRenderItemFlag(unordered_map<string, FxRenderItem>& input, const char* text, bool flag) {
		auto it = input.find(text);
		if (it != input.end())
			it->second.ExecuteProgramFlag = flag;
	}

	uint32_t FindRawTextureImg(unordered_map<string, FStextureHandle>& input, const char* text) {
		return ((input.find(text) != input.end()) ? input[text] : FStextureHandle()).texture_hd;
	}

	void StaticFlagAlloc(unordered_map<string, bool>& input, const char* text, bool init, bool REPEAT) {
		auto it = input.find(text);
		if (it != input.end() && REPEAT)
			LogInfoPush(LOG_WARN, SYSTEM_MODULE_LABLE_render + "alloc static_flag repeat_key: " + text);
		else {
			input[text] = init;
			LogInfoPush(LOG_INFO, SYSTEM_MODULE_LABLE_render + "alloc static_flag key: " + text);
		}
	}

	void StaticFlagDelete(unordered_map<string, bool>& input, const char* text) {
		auto it = input.find(text);
		if (it != input.end()) {
			input.erase(it);
			LogInfoPush(LOG_INFO, SYSTEM_MODULE_LABLE_render + "delete static_flag key: " + text);
		}
		else
			LogInfoPush(LOG_INFO, SYSTEM_MODULE_LABLE_render + "delete static_flag not found key.");
	}

	bool* StaticFlagFind(unordered_map<string, bool>& input, const char* text) {
		return (input.find(text) != input.end()) ? &(input[text]) : nullptr;
	}
}

namespace CoreModuleRender {

	int32_t FrameworkRender::ProfxEventLoop() {
		bool LogicRetFlag = false;

		RenderContextA();
		RenderGuiContextA();
		{
			RenderSceneFBO();
			ImGuiPro::FullScreenBackground(
				ImFxFind::FindRenderItemFBO(DataFxRender, "fx_background_shader")
			);
			LogicEventLoop();
		}
		RenderGuiContextB();
		RenderContextB();

		if (CloseFlag() || LogicRetFlag) return 0;
		else return 1;
	}
}