// framework_core_loop.
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
			// new_frame update fbo, update module.
			UpdateFXshaderFrameBuffer();
			UpdateCoreModuleObject();

			// bg_shader => find_texture => bg_func.
			BackgroundFunction(LLRES_Textures->ResourceFind(COREDATAPACK.BackgroundShaderFX.FrameBufferTexItem).Texture);

			// render shader => draw background => draw imgui logic.
			for (auto it = COREDATAUSEROBJ.begin(); it != COREDATAUSEROBJ.end(); ++it)
				LogicRetFlag |= !it->second->LogicEventLoop(COREDATAPACK, COREDATAINFO);

			// OLD: ImGui::PopID, NEW: counter reset.
			SYSIMGUI_COUPLING::ImGuiControlUniqueIDcount = NULL;
		}
		// ******************************** FBO捕获全局ImGui图像 ********************************
		RenderingSupport::RnenderBindFrameBuffer(LLRES_FrameBuffers->ResourceFind(GuiFxProcess.FrameBufferItem));
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		{
			// draw imgui => opengl.
			RenderGuiContextB();
		}
		RenderingSupport::RnenderUnbindFrameBuffer();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// ******************************** 提交由GuiFinal处理 ********************************
		{
			auto ShaderTemp = LLRES_Shaders->ResourceFind(GuiFxProcess.ShaderProgramItem);
			RenderingSupport::RenderBindShader(ShaderTemp);

			RenderingSupport::RenderDrawModel(LLRES_VertexBuffers->ResourceFind(GuiFxProcess.ModelItem));
			// framework preset uniform.
			LLMAG_Uniform.UniformMatrix4x4(ShaderTemp, "FxMatMvp", GuiFxProcess.RenderMatrixMvp);
			LLMAG_Uniform.UniformVec2(ShaderTemp, "FxWinSize", COREDATAINFO.WindowSize);

			// bloom_radius limit = [0,32]. update: 2024.01.01 RCSZ.
			// framework settings uniform.
			LLMAG_Uniform.UniformInteger(ShaderTemp, "FxSwitch", (int32_t)COREDATAINFO.GuiFinalFxSettings.FxEnableBloom);
			LLMAG_Uniform.UniformInteger(ShaderTemp, "FxBloomRadius", COREDATAINFO.GuiFinalFxSettings.FxBloomRadius);
			LLMAG_Uniform.UniformFloat(ShaderTemp, "BloomFilter", COREDATAINFO.GuiFinalFxSettings.FxBloomFilter);
			LLMAG_Uniform.UniformFloat(ShaderTemp, "BloomBlur", COREDATAINFO.GuiFinalFxSettings.FxBloomColorBlend.vector_x);
			LLMAG_Uniform.UniformFloat(ShaderTemp, "BloomSource", COREDATAINFO.GuiFinalFxSettings.FxBloomColorBlend.vector_y);

			auto TextureTemp = LLRES_Textures->ResourceFind(GuiFxProcess.FrameBufferProcessTex);
			RenderingSupport::RenderBindTexture(TextureTemp);
			// bind texture context => sampler(tmu) => unbind.
			LLMAG_Uniform.UniformInteger(ShaderTemp, "GuiTexture", TextureTemp.TextureSamplerCount);

			RenderingSupport::RenderUnbindShader();
		}
		// ******************************** GLFW交换缓冲绘制到屏幕 ********************************
		RenderContextBelow();

		if (CloseFlag() || LogicRetFlag) return 0;
		else return 1;
	}

	void FrameworkImCore::UpdateFXshaderFrameBuffer() {
		// update fx_shader, timer, draw, framebuffer_oper.
		auto UpdateFxShader = [&](FxRenderGroup& Item) {

			if (Item.ExecuteProgramFlag &&
				chrono::duration_cast<chrono::microseconds>(chrono::steady_clock::now() - Item.RenderRateTimer).count() >
				Item.RenderRateTime)
			{
				// clear item timer.
				Item.RenderRateTimer = chrono::steady_clock::now();
				// bind framebuffer => context => texture.
				RenderingSupport::RnenderBindFrameBuffer(LLRES_FrameBuffers->ResourceFind(Item.FrameBufferItem));
				{
					// clear render buffer_frame.
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

					auto ShaderTemp = LLRES_Shaders->ResourceFind(Item.ShaderProgramItem);
					// bind shader_program context.
					RenderingSupport::RenderBindShader(ShaderTemp);
					// draw model(vao,vbo).
					RenderingSupport::RenderDrawModel(LLRES_VertexBuffers->ResourceFind(Item.ModelItem));

					LLMAG_Uniform.UniformMatrix4x4(ShaderTemp, "FxMatMvp", Item.RenderMatrixMvp);
					LLMAG_Uniform.UniformFloat(ShaderTemp, "FxTime", Item.RenderTimer);
					LLMAG_Uniform.UniformVec2(ShaderTemp, "FxWinSize", COREDATAINFO.WindowSize);

					// external uniform.
					for (auto itprm = Item.ParamValueFloat.begin(); itprm != Item.ParamValueFloat.end(); ++itprm)
						LLMAG_Uniform.UniformFloat(ShaderTemp, itprm->first.c_str(), itprm->second);

					// external sample texture.
					for (auto& ittex : Item.ParamTextureItems) {
						auto TextureTemp = LLRES_Textures->ResourceFind(ittex);

						RenderingSupport::RenderBindTexture(TextureTemp);
						// bind texture context => sampler(tmu) => unbind.
						LLMAG_Uniform.UniformInteger(ShaderTemp, ittex.c_str(), TextureTemp.TextureSamplerCount);
						// RenderingSupport::RenderUnbindTexture();
					}
					// unbind shader_program context.
					RenderingSupport::RenderUnbindShader();
				}
				RenderingSupport::RnenderUnbindFrameBuffer();
				// clear render buffer_frame.
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				// shader virtual tick.
				Item.RenderTimer += RENDERLOOP_TIMER_STEP * COREDATAINFO.FrameSmoothValue;
			}
		};

		for (auto it = COREDATAPACK.DataShaderFx.begin(); it != COREDATAPACK.DataShaderFx.end(); ++it)
			UpdateFxShader(it->second);
		UpdateFxShader(COREDATAPACK.BackgroundShaderFX);

		// [2023_10_31] reference_fps: 60, sample_timer: 250 ms
		CalculateSmoothTrans(60.0f, 250);
		// clear unique id.
		*COREDATAINFO.S_UniqueCountID = DefaultUniqueID;
		COREDATAINFO.S_RandomFunction = &GenerateRandomNumbers;

		// GLFW data => Framework "COREDATAINFO".
		COREDATAINFO.WindowSize    = ValueWindowSize;
		COREDATAINFO.MousePosition = ValueMouseCursor;
		COREDATAINFO.WindowFoucs   = ValueWindowFocus;
		COREDATAINFO.DropFiles     = &FilePaths;
	}

	void FrameworkImCore::UpdateCoreModuleObject() {
		// update_module: animation object.
		for (auto it = COREDATAPACK.DataAnimation.begin(); it != COREDATAPACK.DataAnimation.end(); ++it) {
			// global imgui unique_id.
			*it->second->_MS_UNIQUE() = SYSIMGUI_COUPLING::ImGuiControlUniqueIDcount;
			++SYSIMGUI_COUPLING::ImGuiControlUniqueIDcount;
		}

		// update_module: memory object.
		for (auto it = COREDATAPACK.DataDynamicMem.begin(); it != COREDATAPACK.DataDynamicMem.end(); ++it)
			it->second->_UPDATE_MemorySystem();
		// update_module: particle object.
		for (auto it = COREDATAPACK.ParticleSystem.begin(); it != COREDATAPACK.ParticleSystem.end(); ++it)
			it->second->_UPDATE_ParticleSystem();
	}

	void FrameworkImCore::CalculateSmoothTrans(float preset_fps, int64_t sample_time) {
		if (chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - SampleTimer).count() > sample_time) {
			
			TimerSmoothValue = preset_fps / (TimerSmoothCount * 1000.0f / (float)sample_time);
			TimerSmoothCount = NULL;
			SampleTimer = chrono::steady_clock::now();
		}
		TimerSmoothCount += 1.0f;
		COREDATAINFO.FrameSmoothValue += (TimerSmoothValue - COREDATAINFO.FrameSmoothValue) * 0.072f;
	}

	float FrameworkImCore::GenerateRandomNumbers(float value_min, float value_max) {
		// random_device(HRNG) => mt19937, 20231120 RCSZ.
		random_device CreateRandom;
		mt19937 MtGen(CreateRandom());
		uniform_real_distribution<float> Distr(value_min, value_max);
		return Distr(MtGen);
	}
}