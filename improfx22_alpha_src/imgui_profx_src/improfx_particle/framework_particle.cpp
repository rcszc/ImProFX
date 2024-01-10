// framework_particle.
#include <random>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "framework_particle.h"

using namespace std;
using namespace LOGCONS;
using namespace IFC_PARTC;

#define DEG_TO_RAD(degrees) ((degrees) * 3.14159265 / 180.0)

#define PTCMS_SIN(deg) (float)sin(DEG_TO_RAD(deg))
#define PTCMS_COS(deg) (float)cos(DEG_TO_RAD(deg))

constexpr size_t ParticleVertAtt = 12;
constexpr size_t ParticleVertLen = 72;

inline float random_timeseed_fp32(float value_min, float value_max) {
	// seed: time(microseconds) => MT19937.
	mt19937 MtGen((uint32_t)chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count());
	uniform_real_distribution<float> Distr(value_min, value_max);
	return Distr(MtGen);
}

inline void particle_disp_pos(ParticleAttributes& ptc, const Vector2T<float>& pos, const Vector3T<float>& off) {
	// position dispersion.
	ptc.ParticlePosition.vector_x = random_timeseed_fp32(pos.vector_x, pos.vector_y) + off.vector_x;
	ptc.ParticlePosition.vector_y = random_timeseed_fp32(pos.vector_x, pos.vector_y) + off.vector_y;
	ptc.ParticlePosition.vector_z = random_timeseed_fp32(pos.vector_x, pos.vector_y) + off.vector_z;
}

inline void particle_disp_vec(ParticleAttributes& ptc, Vector2T<float>& vec) {
	// vector dispersion.
	ptc.ParticleVector.vector_x = random_timeseed_fp32(vec.vector_x, vec.vector_y);
	ptc.ParticleVector.vector_y = random_timeseed_fp32(vec.vector_x, vec.vector_y);
	ptc.ParticleVector.vector_z = random_timeseed_fp32(vec.vector_x, vec.vector_y);
}

inline void particle_colorsys(ParticleAttributes& ptc, const Vector3T<bool>& colorsys) {
	// color system.
	ptc.ParticleColor.vector_x = float(rand() % (1500 - 500 + 1) + 500) / 1000.0f * (float)colorsys.vector_x;
	ptc.ParticleColor.vector_y = float(rand() % (1500 - 500 + 1) + 500) / 1000.0f * (float)colorsys.vector_y;
	ptc.ParticleColor.vector_z = float(rand() % (1500 - 500 + 1) + 500) / 1000.0f * (float)colorsys.vector_z;
	ptc.ParticleColor.vector_w = float(rand() % (1500 - 500 + 1) + 500) / 1000.0f;
}

namespace ImParticle {

	void ImPTCgenerator::ConfigCreateNumber(float number) {
		if (number < 8.0f)
			PushLogger(LOG_WARNING, SYSMD_LB_PARTICLE, "generator particles < 8.");
		else
			ParticlesNumber = (size_t)number;
	}

	void ImPTCgenerator::ConfigCreateMode(ParticleCreateMode mode) {
		CreateMode = mode;
	}

	void ImPTCgenerator::ConfigLifeDispersion(Vector2T<float> rand_limit_life) {
		// limit > 0.0f, 0.0f:free_particle.
		rand_limit_life.vector_x = rand_limit_life.vector_x <= 0.0f ? 0.0f : rand_limit_life.vector_x;
		rand_limit_life.vector_y = rand_limit_life.vector_y <= 0.0f ? 0.0f : rand_limit_life.vector_y;
		RandomLimitLife = Vector2T<uint32_t>((uint32_t)rand_limit_life.vector_x, (uint32_t)rand_limit_life.vector_y);
	}

	void ImPTCgenerator::ConfigRandomDispersion(Vector2T<float> rand_limit_vector, Vector2T<float> rand_limit_position, Vector3T<bool> color_system, Vector3T<float> offset_position) {
		RandomLimitVector = rand_limit_vector; RandomLimitPosition = rand_limit_position;
		ColorSystemSw = color_system; OffsetPosition = offset_position;
	}

	void ImPTCgenerator::CreateAddParticleDataset(vector<ParticleAttributes>& Data) {
		switch (CreateMode) {
		// "VecRandLimit" x,y [speed]
		case(PTCpoints): {
			for (size_t i = 0; i < ParticlesNumber; ++i) {
				ParticleAttributes ParticleTemp = {};

				particle_disp_pos(ParticleTemp, RandomLimitPosition, OffsetPosition);
				particle_disp_vec(ParticleTemp, RandomLimitVector);
				particle_colorsys(ParticleTemp, ColorSystemSw);

				ParticleTemp.ParticleLife = random_timeseed_fp32((float)RandomLimitLife.vector_x, (float)RandomLimitLife.vector_y);
				ParticlesGenCache.push_back(ParticleTemp);
			}
			break;
		}
		// "VecRandLimit" x:cricle_x_scale, y:cricle_y_scale [speed]
		case(PTCcircle): {
			for (size_t i = 0; i < ParticlesNumber; ++i) {
				ParticleAttributes ParticleTemp = {};

				particle_disp_pos(ParticleTemp, RandomLimitPosition, OffsetPosition);
				particle_colorsys(ParticleTemp, ColorSystemSw);

				ParticleTemp.ParticleVector = 
					Vector3T<float>(PTCMS_SIN(i * 3.8f) * RandomLimitVector.vector_x, PTCMS_COS(i * 3.8f) * RandomLimitVector.vector_y, 0.0f);

				ParticleTemp.ParticleLife = random_timeseed_fp32((float)RandomLimitLife.vector_x, (float)RandomLimitLife.vector_y);
				ParticlesGenCache.push_back(ParticleTemp);
			}
			break;
		}
		// "VecRandLimit" x:square_side, y:null [speed] 
		case(PTCsquare): {
			const float CenterX = 0.0f, CenterY = 0.0f;
			vector<Vector3T<float>> PointsSample = {};

			float SideLength = RandomLimitVector.vector_x;
			float SampleStep = SideLength / (float)ParticlesNumber * 4.0f;

			for (int i = 0; i < ParticlesNumber / 4; ++i) {
				PointsSample.push_back(Vector3T<float>(-(SideLength / 2.0f) + (float)i * SampleStep, -(SideLength / 2.0f), 0.0f));
				PointsSample.push_back(Vector3T<float>(SideLength / 2.0f, -(SideLength / 2.0f) + (float)i * SampleStep, 0.0f));
				PointsSample.push_back(Vector3T<float>(-(SideLength / 2.0f) + (float)i * SampleStep, SideLength / 2.0f, 0.0f));
				PointsSample.push_back(Vector3T<float>(-(SideLength / 2.0f), -(SideLength / 2.0f) + (float)i * SampleStep, 0.0f));
			}

			for (const auto& Item : PointsSample) {
				ParticleAttributes ParticleTemp = {};

				particle_disp_pos(ParticleTemp, RandomLimitPosition, OffsetPosition);
				particle_colorsys(ParticleTemp, ColorSystemSw);

				ParticleTemp.ParticleVector = Item;

				ParticleTemp.ParticleLife = random_timeseed_fp32((float)RandomLimitLife.vector_x, (float)RandomLimitLife.vector_y);
				ParticlesGenCache.push_back(ParticleTemp);
			}
			break;
		}
		// "VecRandLimit" x:abs(y_scale), y [speed]
		// "OffsetPosition" x, y:high(offset)
		case(PTCdrift): {
			for (size_t i = 0; i < ParticlesNumber; ++i) {
				ParticleAttributes ParticleTemp = {};

				particle_colorsys(ParticleTemp, ColorSystemSw);

				ParticleTemp.ParticlePosition.vector_y = random_timeseed_fp32(-700.0f, -300.0f) + OffsetPosition.vector_y;
				ParticleTemp.ParticlePosition.vector_x = random_timeseed_fp32(RandomLimitPosition.vector_x, RandomLimitPosition.vector_y) + OffsetPosition.vector_x;
				ParticleTemp.ParticlePosition.vector_z = random_timeseed_fp32(RandomLimitPosition.vector_x, RandomLimitPosition.vector_y) + OffsetPosition.vector_z;

				particle_disp_vec(ParticleTemp, RandomLimitVector);
				ParticleTemp.ParticleVector.vector_y = abs(RandomLimitVector.vector_x);

				ParticleTemp.ParticleLife = random_timeseed_fp32((float)RandomLimitLife.vector_x, (float)RandomLimitLife.vector_y);
				ParticlesGenCache.push_back(ParticleTemp);
			}
			break;
		}
		}
		Data.insert(Data.end(), ParticlesGenCache.begin(), ParticlesGenCache.end());
		PushLogger(LOG_INFO, SYSMD_LB_PARTICLE, "create particles: %u", ParticlesGenCache.size());
	}

#define DEFAULT_SPEED 1.0f
	void ImParticleFx::CalcUpdateParticles(vector<ParticleAttributes>& ptc, float speed, float lifesub) {

		for (size_t i = 0; i < ptc.size(); ++i) {
			// update position.
			ptc[i].ParticlePosition.vector_x += ptc[i].ParticleVector.vector_x * DEFAULT_SPEED * speed;
			ptc[i].ParticlePosition.vector_y += ptc[i].ParticleVector.vector_y * DEFAULT_SPEED * speed;
			ptc[i].ParticlePosition.vector_z += ptc[i].ParticleVector.vector_z * DEFAULT_SPEED * speed;

			ptc[i].ParticleLife -= DEFAULT_SPEED * lifesub;
			// delete particle.
			if (ptc[i].ParticleLife <= 0.0f)
				ptc.erase(ptc.begin() + i);
		}
	}

	void ImParticleFx::VertexDataConvert(const vector<ParticleAttributes>& src, vector<float>& cvt) {
		Vector4T<float> ColorTemp = {};
		Vector3T<float> PositionTemp = {};
		// clear vertex srcdata_cache => convert.
		cvt.clear();
		for (const auto& Item : src) {

			// create particle: position,color.
			PositionTemp = Item.ParticlePosition;
			ColorTemp = Item.ParticleColor;

			// format: vertex[vec3], color[vec4], uv(coord)[vec2], normal[vec3].
			float DefaultParticle[72] = {
				-5.0f + PositionTemp.vector_x, -5.0f + PositionTemp.vector_y, 0.0f + PositionTemp.vector_z,
				 0.0f + ColorTemp.vector_x, 0.0f + ColorTemp.vector_y, 0.0f + ColorTemp.vector_z, 0.0f + ColorTemp.vector_w,
				 0.0f,0.0f, 0.0f,0.0f,0.0f,
				 5.0f + PositionTemp.vector_x, -5.0f + PositionTemp.vector_y, 0.0f + PositionTemp.vector_z,
				 0.0f + ColorTemp.vector_x, 0.0f + ColorTemp.vector_y, 0.0f + ColorTemp.vector_z, 0.0f + ColorTemp.vector_w,
				 1.0f,0.0f, 0.0f,0.0f,0.0f,
				 5.0f + PositionTemp.vector_x,  5.0f + PositionTemp.vector_y, 0.0f + PositionTemp.vector_z,
				 0.0f + ColorTemp.vector_x, 0.0f + ColorTemp.vector_y, 0.0f + ColorTemp.vector_z, 0.0f + ColorTemp.vector_w,
				 1.0f,1.0f, 0.0f,0.0f,0.0f,

				-5.0f + PositionTemp.vector_x, -5.0f + PositionTemp.vector_y, 0.0f + PositionTemp.vector_z,
				 0.0f + ColorTemp.vector_x, 0.0f + ColorTemp.vector_y, 0.0f + ColorTemp.vector_z, 0.0f + ColorTemp.vector_w,
				 0.0f,0.0f, 0.0f,0.0f,0.0f,
				 5.0f + PositionTemp.vector_x,  5.0f + PositionTemp.vector_y, 0.0f + PositionTemp.vector_z,
				 0.0f + ColorTemp.vector_x, 0.0f + ColorTemp.vector_y, 0.0f + ColorTemp.vector_z, 0.0f + ColorTemp.vector_w,
				 1.0f,1.0f, 0.0f,0.0f,0.0f,
				-5.0f + PositionTemp.vector_x,  5.0f + PositionTemp.vector_y, 0.0f + PositionTemp.vector_z,
				 0.0f + ColorTemp.vector_x, 0.0f + ColorTemp.vector_y, 0.0f + ColorTemp.vector_z, 0.0f + ColorTemp.vector_w,
				 0.0f,1.0f, 0.0f,0.0f,0.0f
			};
			// src => particle vert => cvt
			cvt.insert(cvt.end(), DefaultParticle, DefaultParticle + ParticleVertLen);
		}
	}

	ImParticleFx::ImParticleFx(Vector2T<float> window_resolution) {
		IMPROFX_GEN_NUMBER GenResID;
		auto GenResourceID = [&]() { return to_string(GenResID.IPFGEN_GenNumberTime()); };

		// ******************************** create shader_program ********************************
		IMFX_OGL_MAG::ImFXOGLshader ShaderProcess;
		ShaderProgramItem = GenResourceID();

		ShaderProcess.ShaderLoaderVS(particle_shader_script::FxShaderDefaultVert, StringScript);
		ShaderProcess.ShaderLoaderFS(particle_shader_script::FxShaderDefaultFrag, StringScript);

		if (ShaderProcess.CreateCompileShaders())
			LLRES_Shaders->ResourceStorage(ShaderProgramItem, &ShaderProcess);

		// ******************************** create model ********************************
		IMFX_OGL_MAG::ImFXOGLmodel ModelProcess;
		VertexModelItem = GenResourceID();

		auto VerBufferTemp = ModelProcess.CreateVertexBuffer();
		
		// 在第一次创建粒子系统时单独创建'VAO'.
		if (!LLRES_VertexAttributes->ResourceFind("VER_ATTR2"))
			LLRES_VertexAttributes->ResourceStorage("VER_ATTR2", ModelProcess.CreateVertexAttribute(0, 0));
		
		if (ModelProcess.CreateDynamicModel(LLRES_VertexAttributes->ResourceFind("VER_ATTR2"), VerBufferTemp, nullptr, NULL))
			LLRES_VertexBuffers->ResourceStorage(VertexModelItem, &ModelProcess);

		float RenderScale = window_resolution.vector_x / window_resolution.vector_y;

		glm::mat4 MatrixProj = glm::ortho(-SpaceSize * RenderScale, SpaceSize * RenderScale, -SpaceSize, SpaceSize, -SpaceSize, SpaceSize);
		glm::mat4 MVPmatrixTemp = MatrixProj; // * view_matrix * model_matrix

		// convert: glm matrix => imfx matrix.
		const float* glmmatptr = glm::value_ptr(MVPmatrixTemp);
		memcpy_s(RenderMatrix.matrix, 16 * sizeof(float), glmmatptr, 16 * sizeof(float));

		// ******************************** create texture & framebuffer ********************************
		IMFX_OGL_MAG::ImFXOGLtexture FboTexProcess;
		IMFX_OGL_MAG::ImFXOGLframebuffer FboProcess;

		FrameBufferTexItem = GenResourceID();
		FrameBufferItem    = GenResourceID();

		if (FboTexProcess.CreateTextureEmpty((uint32_t)window_resolution.vector_x, (uint32_t)window_resolution.vector_y, 4, LinearFiltering))
			LLRES_Textures->ResourceStorage(FrameBufferTexItem, &FboTexProcess);

		if (FboProcess.CreateBindTexture(LLRES_Textures->ResourceFind(FrameBufferTexItem)))
			LLRES_FrameBuffers->ResourceStorage(FrameBufferItem, &FboProcess);

		PushLogger(LOG_TRACE, SYSMD_LB_PARTICLE, "particle system init.");
		PushLogger(LOG_INFO, SYSMD_LB_PARTICLE, "particle system render_resolution: %.2f x %.2f", window_resolution.vector_x, window_resolution.vector_y);
	}

	ImParticleFx::~ImParticleFx() {
		// delete particles res.
		LLRES_VertexBuffers->ResourceDelete(VertexModelItem);
		LLRES_Textures->ResourceDelete(FrameBufferTexItem);
		LLRES_FrameBuffers->ResourceDelete(FrameBufferItem);
		LLRES_Shaders->ResourceDelete(ShaderProgramItem);
	}

	void ImParticleFx::ParticleSystemUpdate() {
		// calculate particle state.
		CalcUpdateParticles(DataParticles, 0.5f, 1.0f);
		// "particle_attributes" => float
		VertexDataConvert(DataParticles, DataVertices);
		// convert => update vert.
		RenderingSupport::UpdateModel(LLRES_VertexBuffers->ExtResourceMapping(VertexModelItem), DataVertices.data(), DataVertices.size() * sizeof(float));
		
		RenderingSupport::RnenderFrameBufferA(LLRES_FrameBuffers->ResourceFind(FrameBufferItem));
		{
			// clear render buffer_frame.
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			auto ShaderProgramTemp = LLRES_Shaders->ResourceFind(ShaderProgramItem);
			RenderingSupport::RenderBindShader(ShaderProgramTemp);
			// draw particles vert.
			RenderingSupport::RenderDrawModel(LLRES_VertexBuffers->ResourceFind(VertexModelItem));
			
			// system parset uniform.
			ShaderUniform.UniformMatrix4x4(ShaderProgramTemp, "FxMatMvp", RenderMatrix);
			ShaderUniform.UniformFloat(ShaderProgramTemp, "FxTime", RenderTimer);

			RenderingSupport::RenderUnbindShader();
		}
		RenderingSupport::RnenderFrameBufferB();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		RenderTimer += RENDERLOOP_TIMER_STEP;
	}

	ParticleSystemState ImParticleFx::GetParticleState() {
		ParticleSystemState ResultState = {};

		ResultState.DarwParticlesNumber = DataParticles.size();
		ResultState.DarwDatasetSize = DataVertices.size();

		return ResultState;
	}

	uint32_t ImParticleFx::GetParticleRenderHandle() {
		// get particle_system FBO handle.
		return LLRES_Textures->ResourceFind(FrameBufferTexItem).Texture;
	}

	void ImParticleFx::ParticleCreate(ParticleCreateBase* generator) {
		// generator => particle_system.
		generator->CreateAddParticleDataset(DataParticles);
	}

	vector<ParticleAttributes>* ImParticleFx::GetParticleDataset() {
		// particle_system ptc_data pointer.
		return &DataParticles;
	}
	/*
	* 粒子系统单独占用1个'VAO'.
	* 粒子系统相对'LLRES'的二级映射资源索引由自身管理.
	*/
}