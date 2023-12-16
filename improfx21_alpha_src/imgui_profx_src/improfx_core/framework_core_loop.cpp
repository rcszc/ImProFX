// framework_core_loop.
#include <random>

#include "framework_core.hpp"

using namespace std;
using namespace LOGCONS;

constexpr uint32_t DefaultShaderHD = 0;
constexpr uint32_t DefaultUniqueID = 0;

namespace CoreModuleIMFX {

	int32_t FrameworkImCore::ProFxCoreEventloop() {
		bool LogicRetFlag = false;
		
		RenderContextAbove(); 
		RenderGuiContextA();
		{
			UpdateFXshaderFrameBuffer();
			BackgroundFunction();
			// render shader => draw background => draw imgui logic.
			for (auto it = COREDATAUSEROBJ.begin(); it != COREDATAUSEROBJ.end(); ++it)
				LogicRetFlag |= !it->second->LogicEventLoop(COREDATAPACK, COREDATAINFO);
		}
		RenderGuiContextB(); 
		RenderContextBelow();

		if (CloseFlag() || LogicRetFlag) return 0;
		else return 1;
	}

	void FrameworkImCore::UpdateFXshaderFrameBuffer() {
		for (auto it = COREDATAPACK.DataShaderFx.begin(); it != COREDATAPACK.DataShaderFx.end(); ++it) {

			if (it->second.ExecuteProgramFlag &&
				chrono::duration_cast<chrono::microseconds>(chrono::steady_clock::now() - it->second.RenderRateTimer).count() >
				it->second.RenderRateTime
				) {
				it->second.RenderRateTimer = chrono::steady_clock::now(); // clear item timer.

				FS_FrameBufferAbove(it->second.FrameBufferHandle);
				FS_ClearFrameBuffer();

				FS_ShaderEnable(it->second.ShaderProgramHandle);
				FS_ShaderDrawMD(it->second.ModelHandle, 6);

				// system param.
				SFglobalUniform.uniform_matrix4x4(it->second.ShaderProgramHandle, "FxMatMvp", it->second.RenderMatrixMvp);
				SFglobalUniform.uniform_vec2(it->second.ShaderProgramHandle, "FxWinSize", COREDATAINFO.WindowSize);
				SFglobalUniform.uniform_float(it->second.ShaderProgramHandle, "FxTime", it->second.RenderTimer);

				// custom param.
				for (auto itprm = it->second.ParamValueFloat.begin(); itprm != it->second.ParamValueFloat.end(); ++itprm)
					SFglobalUniform.uniform_float(it->second.ShaderProgramHandle, itprm->first.c_str(), itprm->second);

				for (auto& res_texture : it->second.ParamTexture)
					FS_ShaderTexture(res_texture.SamplerCount, res_texture);

				FS_ShaderEnable(DefaultShaderHD);
				FS_FrameBufferBelow();
				FS_ClearFrameBuffer();

				it->second.RenderTimer += RENDERLOOP_TIMER_STEP * COREDATAINFO.FrameSmoothValue;
			}
		}
		// [2023_10_31] reference_fps: 60, sample_timer: 250 ms
		CalculateSmoothTrans(60.0f, 250);
		// clear unique id.
		COREDATAINFO._UniqueCountID  = DefaultUniqueID;
		COREDATAINFO._RandomFunction = &GenerateRandomNumbers;

		// GLFW data => Framework "COREDATAINFO".
		COREDATAINFO.WindowSize    = ValueWindowSize;
		COREDATAINFO.MousePosition = ValueMouseCursor;
		COREDATAINFO.WindowFoucs   = ValueWindowFocus;
		COREDATAINFO.DropFiles     = &FilePaths;
	}

	void FrameworkImCore::UpdateCoreModuleObject() {
		// update_module: memory object.
		for (auto it = COREDATAPACK.DataDynamicMem.begin(); it != COREDATAPACK.DataDynamicMem.end(); ++it)
			it->second->MemSystemUpdate();
	}

	void FrameworkImCore::CalculateSmoothTrans(float preset_fps, int64_t sample_time) {
		if (chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - SampleTimer).count() > sample_time) {
			
			TimerSmoothValue = preset_fps / (TimerSmoothCount * 1000.0f / (float)sample_time);
			TimerSmoothCount = NULL;
			SampleTimer = chrono::steady_clock::now();
		}
		TimerSmoothCount += 1.0f;
		COREDATAINFO.FrameSmoothValue += (TimerSmoothValue - COREDATAINFO.FrameSmoothValue) * 0.0072f;
	}

	float FrameworkImCore::GenerateRandomNumbers(float value_min, float value_max) {
		// random_device(HRNG) => mt19937, 20231120 RCSZ.
		random_device CreateRandom;
		mt19937 _GEN(CreateRandom());
		uniform_real_distribution<float> _DIS(value_min, value_max);
		return _DIS(_GEN);
	}
}