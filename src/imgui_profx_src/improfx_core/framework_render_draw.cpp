// framework_render_draw.
#include "framework_render.h"

using namespace std;
using namespace LOGCONS;

uint32_t default_shader_hd = 0;

namespace CoreModuleRender {

	void FrameworkRender::RenderSceneFBO() {
		for (auto it = DataFxRender.begin(); it != DataFxRender.end(); ++it) {
			if (it->second.ExecuteProgramFlag) {

				FS_FrameBufferAbove(it->second.FrameBufferHandle);
				FS_ClearFrameBuffer();

				FS_ShaderEnable(it->second.ShaderProgramHandle);
				FS_ShaderDrawMD(it->second.ModelHandle);

				// system param.
				SFglobalUniform.sUniformMat4(it->second.ShaderProgramHandle, "system_mvp_matrix", it->second.RenderMVPmatrix);
				SFglobalUniform.sUniform2f(it->second.ShaderProgramHandle, "system_window_size", WindowSize);
				SFglobalUniform.sUniform1f(it->second.ShaderProgramHandle, "system_tick", it->second.RenderTimer);

				// custom param.
				for (auto itprm = it->second.ParamValueFloat.begin(); itprm != it->second.ParamValueFloat.end(); ++itprm)
					SFglobalUniform.sUniform1f(it->second.ShaderProgramHandle, itprm->first.c_str(), itprm->second);

				uint32_t idx_count = NULL;
				for (auto& res_texture : it->second.ParamTexture) {
					FS_ShaderTexture(idx_count, res_texture);
					++idx_count;
					// sample unit number limit.
					idx_count = idx_count >= MaxTextureSample ? MaxTextureSample - 1 : idx_count;
				}

				FS_FrameBufferBelow();
				FS_ClearFrameBuffer();
				FS_ShaderEnable(default_shader_hd);

				it->second.RenderTimer += DRAW_TIMER_COUNT * TimerSmoothValue[1];
			}
		}
		// [2023_10_31] reference_fps: 60, sample_timer: 250 ms
		calculate_smooth_trans(60.0f, 250);
	}

	void FrameworkRender::calculate_smooth_trans(float preset_fps, int64_t sample_time) {
		if (chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - SampleTimer).count() > sample_time) {
			
			TimerSmoothValue[0] = preset_fps / (TimerSmoothCount * 1000.0f / (float)sample_time);
			TimerSmoothCount = NULL;
			SampleTimer = chrono::steady_clock::now();
		}
		TimerSmoothCount += 1.0f;
		TimerSmoothValue[1] += (TimerSmoothValue[0] - TimerSmoothValue[1]) * 0.0072f;
	}
}