// framework_particle.

#ifndef _FRAMEWORK_PARTICLE_H
#define _FRAMEWORK_PARTICLE_H
#include "framework_particle_interface.h"

#define SYSMD_LB_PARTICLE "[IM_PARTICLE]: "

namespace particle_shader_script {
	extern const char* FxShaderDefaultVert;
	extern const char* FxShaderDefaultFrag;
}

namespace ImParticle {

	class ImPTCgenerator :public IFC_PARTC::ParticleCreateBase {
	protected:
		std::vector<IFC_PARTC::ParticleAttributes> ParticlesGenCache = {};

		ParticleCreateMode CreateMode = {};
		size_t ParticlesNumber = 8;

		Vector3T<bool>     ColorSystemSw   = {};
		Vector2T<uint32_t> RandomLimitLife = {};

		Vector3T<float> OffsetPosition      = {};
		Vector2T<float> RandomLimitPosition = {}, RandomLimitVector = {};

	public:
		void ConfigCreateNumber(float number);
		void ConfigCreateMode(ParticleCreateMode mode);
		void ConfigLifeDispersion(Vector2T<float> rand_limit_life = Vector2T<float>(280.0f, 520.0f));
		void ConfigRandomDispersion(
			Vector2T<float> rand_limit_vector,   // vec random[min,max] pos += vec * speed.
			Vector2T<float> rand_limit_position, // pos random[min,max].
			Vector3T<bool>  color_system,        // color system(random) channels_switch.
			Vector3T<float> offset_position = Vector3T<float>()
		);

		void CreateAddParticleDataset(std::vector<IFC_PARTC::ParticleAttributes>& Data);
	};

	class ImParticleFx :public IMPROFX_LOW::IMPROFX_DATASET_LLRES, public IFC_PARTC::ParticleBase {
	protected:
		IMFX_OGL_MAG::ImFxOGLuniform ShaderUniform = {};
		const float SpaceSize = 360.0f;

		std::vector<IFC_PARTC::ParticleAttributes> DataParticles = {};
		std::vector<float>                         DataVertices  = {};

		std::string ShaderProgramItem = {};
		std::string VertexAttrItem    = {};
		std::string VertexModelItem   = {};

		std::string FrameBufferItem    = {};
		std::string FrameBufferTexItem = {};

		float     RenderTimer  = 0.0f;
		FsMatrix4 RenderMatrix = {};

		void CalcUpdateParticles(std::vector<IFC_PARTC::ParticleAttributes>& ptc, float speed, float lifesub);
		void VertexDataConvert(const std::vector<IFC_PARTC::ParticleAttributes>& src, std::vector<float>& cvt);

	public:
		ImParticleFx(Vector2T<float> window_resolution);
		~ImParticleFx();

		void ParticleSystemUpdate();
		void ParticleCreate(IFC_PARTC::ParticleCreateBase* generator);

		uint32_t									GetParticleRenderHandle();
		IFC_PARTC::ParticleSystemState				GetParticleState();
		std::vector<IFC_PARTC::ParticleAttributes>* GetParticleDataset();
	};
}

#endif