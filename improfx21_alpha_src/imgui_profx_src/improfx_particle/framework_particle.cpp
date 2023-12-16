// framework_particle.
#include <random>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "framework_particle.h"
// rendering graphics low-level.
#include "../improfx_shader/framework_shader.h"

using namespace std;
using namespace LOGCONS;
using namespace IFC_PARTC;

#define DEG_TO_RAD(degrees) ((degrees) * 3.14159265 / 180.0)

#define PTCMS_SIN(deg) (float)sin(DEG_TO_RAD(deg))
#define PTCMS_COS(deg) (float)cos(DEG_TO_RAD(deg))

constexpr size_t ParticleVertAtt = 12;
constexpr size_t ParticleVertLen = 72;

inline float random_timeseed_fp32(float value_min, float value_max) {
	// 以时间(微秒)为种子, MT19937.
	mt19937 _gen((uint32_t)chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count());
	uniform_real_distribution<float> _dis(value_min, value_max);
	return _dis(_gen);
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

	void ImPTCgenerator::ConfigCreateNumber(float Number) {
		if (Number < 8.0f)
			PushLogger(LOG_WARN, SYSMD_LB_PARTICLE, "generator particles < 8.");
		else
			ParticlesNumber = (size_t)Number;
	}

	void ImPTCgenerator::ConfigCreateMode(ParticleCreateMode Mode) {
		CreateMode = Mode;
	}

	void ImPTCgenerator::ConfigLifeDispersion(Vector2T<float> LifeRandLimit) {
		// limit > 0.0f, 0.0f:free_particle.
		LifeRandLimit.vector_x = LifeRandLimit.vector_x <= 0.0f ? 0.0f : LifeRandLimit.vector_x;
		LifeRandLimit.vector_y = LifeRandLimit.vector_y <= 0.0f ? 0.0f : LifeRandLimit.vector_y;
		RandLifeLimit = Vector2T<uint32_t>((uint32_t)LifeRandLimit.vector_x, (uint32_t)LifeRandLimit.vector_y);
	}

	void ImPTCgenerator::ConfigRandomDispersion(Vector2T<float> VecRandLimit, Vector2T<float> PosRandLimit, Vector3T<bool>  ColorSystem, Vector3T<float> OffsetPosition) {
		RandVectorLimit = VecRandLimit; RandPositionLimit = PosRandLimit;
		RandColorSystem = ColorSystem; PositionOffset = OffsetPosition;
	}

	void ImPTCgenerator::CreateAddParticleDataset(vector<ParticleAttributes>& Data) {
		switch (CreateMode) {
		// "VecRandLimit" x,y [speed]
		case(PTCpoints): {
			for (size_t i = 0; i < ParticlesNumber; ++i) {
				ParticleAttributes ParticleTemp = {};

				particle_disp_pos(ParticleTemp, RandPositionLimit, PositionOffset);
				particle_disp_vec(ParticleTemp, RandVectorLimit);
				particle_colorsys(ParticleTemp, RandColorSystem);

				ParticleTemp.ParticleLife = random_timeseed_fp32((float)RandLifeLimit.vector_x, (float)RandLifeLimit.vector_y);
				PariclesCache.push_back(ParticleTemp);
			}
			break;
		}
		// "VecRandLimit" x:cricle_x_scale, y:cricle_y_scale [speed]
		case(PTCcircle): {
			for (size_t i = 0; i < ParticlesNumber; ++i) {
				ParticleAttributes ParticleTemp = {};

				particle_disp_pos(ParticleTemp, RandPositionLimit, PositionOffset);
				particle_colorsys(ParticleTemp, RandColorSystem);

				ParticleTemp.ParticleVector = 
					Vector3T<float>(PTCMS_SIN(i * 3.8f) * RandVectorLimit.vector_x, PTCMS_COS(i * 3.8f) * RandVectorLimit.vector_y, 0.0f);

				ParticleTemp.ParticleLife = random_timeseed_fp32((float)RandLifeLimit.vector_x, (float)RandLifeLimit.vector_y);
				PariclesCache.push_back(ParticleTemp);
			}
			break;
		}
		// "VecRandLimit" x:square_side, y:null [speed] 
		case(PTCsquare): {
			const float CenterX = 0.0f, CenterY = 0.0f;
			vector<Vector3T<float>> PointsSample = {};

			float SideLength = RandVectorLimit.vector_x;
			float SampleStep = SideLength / (float)ParticlesNumber * 4.0f;

			for (int i = 0; i < ParticlesNumber / 4; ++i) {
				PointsSample.push_back(Vector3T<float>(-(SideLength / 2.0f) + (float)i * SampleStep, -(SideLength / 2.0f), 0.0f));
				PointsSample.push_back(Vector3T<float>(SideLength / 2.0f, -(SideLength / 2.0f) + (float)i * SampleStep, 0.0f));
				PointsSample.push_back(Vector3T<float>(-(SideLength / 2.0f) + (float)i * SampleStep, SideLength / 2.0f, 0.0f));
				PointsSample.push_back(Vector3T<float>(-(SideLength / 2.0f), -(SideLength / 2.0f) + (float)i * SampleStep, 0.0f));
			}

			for (const auto& Item : PointsSample) {
				ParticleAttributes ParticleTemp = {};

				particle_disp_pos(ParticleTemp, RandPositionLimit, PositionOffset);
				particle_colorsys(ParticleTemp, RandColorSystem);

				ParticleTemp.ParticleVector = Item;

				ParticleTemp.ParticleLife = random_timeseed_fp32((float)RandLifeLimit.vector_x, (float)RandLifeLimit.vector_y);
				PariclesCache.push_back(ParticleTemp);
			}
			break;
		}
		// "VecRandLimit" x:abs(y_scale), y [speed]
		// "OffsetPosition" x, y:high(offset)
		case(PTCdrift): {
			for (size_t i = 0; i < ParticlesNumber; ++i) {
				ParticleAttributes ParticleTemp = {};

				particle_colorsys(ParticleTemp, RandColorSystem);

				ParticleTemp.ParticlePosition.vector_y = random_timeseed_fp32(-700.0f, -300.0f) + PositionOffset.vector_y;
				ParticleTemp.ParticlePosition.vector_x = random_timeseed_fp32(RandPositionLimit.vector_x, RandPositionLimit.vector_y) + PositionOffset.vector_x;
				ParticleTemp.ParticlePosition.vector_z = random_timeseed_fp32(RandPositionLimit.vector_x, RandPositionLimit.vector_y) + PositionOffset.vector_z;

				particle_disp_vec(ParticleTemp, RandVectorLimit);
				ParticleTemp.ParticleVector.vector_y = abs(RandVectorLimit.vector_x);

				ParticleTemp.ParticleLife = random_timeseed_fp32((float)RandLifeLimit.vector_x, (float)RandLifeLimit.vector_y);
				PariclesCache.push_back(ParticleTemp);
			}
			break;
		}
		}
		Data.insert(Data.end(), PariclesCache.begin(), PariclesCache.end());
		PushLogger(LOG_INFO, SYSMD_LB_PARTICLE, "create particles: %u", PariclesCache.size());
	}

#define DEFAULT_SPEED 1.0f
	void ImParticleFx::calc_update_particles(vector<ParticleAttributes>& ptc, float speed, float lifesub) {

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

	void ImParticleFx::vertex_data_convert(const vector<ParticleAttributes>& src, vector<float>& cvt) {
		Vector3T<float> PositionTemp = {};
		Vector4T<float> ColorTemp    = {};

		cvt.clear();
		for (const auto& Item : src) {

			PositionTemp = Item.ParticlePosition;
			ColorTemp = Item.ParticleColor;

			float DefaultParticle[72] = {
				-5.0f + PositionTemp.vector_x, -5.0f + PositionTemp.vector_y, 0.0f + PositionTemp.vector_z,
				 0.0f + ColorTemp.vector_x, 0.0f + ColorTemp.vector_y, 0.0f + ColorTemp.vector_z, 0.0f + ColorTemp.vector_w, 0.0f, 0.0f, 0.0f,0.0f,0.0f,
				 5.0f + PositionTemp.vector_x, -5.0f + PositionTemp.vector_y, 0.0f + PositionTemp.vector_z,
				 0.0f + ColorTemp.vector_x, 0.0f + ColorTemp.vector_y, 0.0f + ColorTemp.vector_z, 0.0f + ColorTemp.vector_w, 1.0f, 0.0f, 0.0f,0.0f,0.0f,
				 5.0f + PositionTemp.vector_x,  5.0f + PositionTemp.vector_y, 0.0f + PositionTemp.vector_z,
				 0.0f + ColorTemp.vector_x, 0.0f + ColorTemp.vector_y, 0.0f + ColorTemp.vector_z, 0.0f + ColorTemp.vector_w, 1.0f, 1.0f, 0.0f,0.0f,0.0f,

				-5.0f + PositionTemp.vector_x, -5.0f + PositionTemp.vector_y, 0.0f + PositionTemp.vector_z,
				 0.0f + ColorTemp.vector_x, 0.0f + ColorTemp.vector_y, 0.0f + ColorTemp.vector_z, 0.0f + ColorTemp.vector_w, 0.0f, 0.0f, 0.0f,0.0f,0.0f,
				 5.0f + PositionTemp.vector_x,  5.0f + PositionTemp.vector_y, 0.0f + PositionTemp.vector_z,
				 0.0f + ColorTemp.vector_x, 0.0f + ColorTemp.vector_y, 0.0f + ColorTemp.vector_z, 0.0f + ColorTemp.vector_w, 1.0f, 1.0f, 0.0f,0.0f,0.0f,
				-5.0f + PositionTemp.vector_x,  5.0f + PositionTemp.vector_y, 0.0f + PositionTemp.vector_z,
				 0.0f + ColorTemp.vector_x, 0.0f + ColorTemp.vector_y, 0.0f + ColorTemp.vector_z, 0.0f + ColorTemp.vector_w, 0.0f, 1.0f, 0.0f,0.0f,0.0f
			};
			// src => particle vert => cvt
			cvt.insert(cvt.end(), DefaultParticle, DefaultParticle + ParticleVertLen);
		}
	}

	ImParticleFx::ImParticleFx(Vector2T<float> WindowResolution) {
		FSloadShader ParticleShaderScript;

		ParticleShaderScript.load_shader_vs(particle_shader_script::FxShaderDefaultVert, IFC_SHADER::StringScript);
		ParticleShaderScript.load_shader_fs(particle_shader_script::FxShaderDefaultFrag, IFC_SHADER::StringScript);

		ShaderProgramHandle = ParticleShaderScript.create_program_handle();
		ParticleShaderScript.delete_shader();

		VertexModelHandle.VboHandle = SFglobalTransmitData.create_vbo_handle();
		VertexModelHandle.VaoHandle = SFglobalTransmitData.create_vao_handle();

		float RenderScale = WindowResolution.vector_x / WindowResolution.vector_y;

		glm::mat4 MatrixProj = glm::ortho(-SpaceSize * RenderScale, SpaceSize * RenderScale, -SpaceSize, SpaceSize, -SpaceSize, SpaceSize);
		glm::mat4 MVPmatrixTemp = MatrixProj; // * view_matrix

		const float* glmmatptr = glm::value_ptr(MVPmatrixTemp); // get glm matrix ptr.
		memcpy_s(RenderMatrix.matrix, 16 * sizeof(float), glmmatptr, 16 * sizeof(float));

		// render => fbo => texture.
		FScreateTexture CreateFrameTexture(TextureColorRGBA, WindowResolution);

		FSloadFrameBuffer CreateFrameBuffer;
		CreateFrameBuffer.create_bind_framebuffer(CreateFrameTexture.get_texture_handle());

		FrameBufferTexture = FsTextureHandle(CreateFrameTexture.get_texture_handle(), ShaderProgramHandle, NULL, "PTC");
		FrameBufferHandle = CreateFrameBuffer.get_framebuffer_handle();

		CreateFrameTexture.unbind_texture();

		PushLogger(LOG_TRC, SYSMD_LB_PARTICLE, "particle system init.");
		PushLogger(LOG_INFO, SYSMD_LB_PARTICLE, "particle system render_resolution: %.2f x %.2f", WindowResolution.vector_x, WindowResolution.vector_y);
	}

	ImParticleFx::~ImParticleFx() {
		// delete opengl handle.
		SFglobalDeletel.delete_texture_handle(FrameBufferTexture);
		SFglobalDeletel.delete_model_handle(VertexModelHandle);
		SFglobalDeletel.delete_framebuffer_handle(FrameBufferHandle);
		SFglobalDeletel.delete_program_handle(ShaderProgramHandle);
	}

	bool ImParticleFx::ParticleSystemUpdate() {
		// calculate particle state.
		calc_update_particles(DataParticles, 0.5f, 1.0f);
		// "particle_attributes" => float
		vertex_data_convert(DataParticles, DataVertices);
		// convert => update vert.
		SFglobalTransmitData.update_vertex_data(FsVertexGroups(DataVertices.size(), DataVertices.data()), VertexModelHandle);

		FS_FrameBufferAbove(FrameBufferHandle);
		FS_ClearFrameBuffer();

		FS_ShaderEnable(ShaderProgramHandle);
		// draw particles vert.
		FS_ShaderDrawMD(VertexModelHandle, int(DataVertices.size() / ParticleVertAtt));

		// system param.
		SFglobalUniform.uniform_matrix4x4(ShaderProgramHandle, "FxMatMvp", RenderMatrix);
		SFglobalUniform.uniform_float(ShaderProgramHandle, "FxTime", RenderTimer);

		FS_ShaderEnable(NULL);
		FS_FrameBufferBelow();
		FS_ClearFrameBuffer();

		RenderTimer += RENDERLOOP_TIMER_STEP;
		return true;
	}

	ParticleSystemState ImParticleFx::GetParticleState() {
		ParticleSystemState ResultState = {};

		ResultState.DarwParticlesNumber = DataParticles.size();
		ResultState.DarwDatasetSize = DataVertices.size();

		return ResultState;
	}

	void ImParticleFx::ParticleCreate(ParticleCreateBase* Generator) {
		Generator->CreateAddParticleDataset(DataParticles);
	}

	uint32_t ImParticleFx::GetParticleRenderHandle() { 
		return FrameBufferTexture.TextureHandle; 
	}

	vector<ParticleAttributes>* ImParticleFx::GetParticleDataset() {
		return &DataParticles;
	}
}