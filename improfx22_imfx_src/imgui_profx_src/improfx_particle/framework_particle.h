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
		std::vector<IFC_PARTC::ParticleAttributes> PariclesCache = {};

		size_t ParticlesNumber = 8;
		ParticleCreateMode CreateMode = {};

		Vector3T<bool>     RandColorSystem = {};
		Vector2T<uint32_t> RandLifeLimit   = {};

		Vector3T<float> PositionOffset = {};
		Vector2T<float> RandPositionLimit = {}, RandVectorLimit = {};

	public:
		void ConfigCreateNumber(float Number);
		void ConfigCreateMode(ParticleCreateMode Mode);
		void ConfigLifeDispersion(Vector2T<float> LifeRandLimit = Vector2T<float>(280.0f, 520.0f));
		void ConfigRandomDispersion(
			Vector2T<float> VecRandLimit,
			Vector2T<float> PosRandLimit,
			Vector3T<bool>  ColorSystem,
			Vector3T<float> OffsetPosition = Vector3T<float>()
		);

		void CreateAddParticleDataset(std::vector<IFC_PARTC::ParticleAttributes>& Data);
	};

	class ImParticleFx :public IMPROFX_LOW::IMPROFX_DATASET_LLRES, public IFC_PARTC::ParticleBase {
	protected:
		IMFX_OGL_MAG::ImFxOGLuniform ShaderUniform = {};
		const float SpaceSize = 384.0f;

		std::vector<IFC_PARTC::ParticleAttributes> DataParticles = {};
		std::vector<float>                         DataVertices  = {};

		std::string ShaderProgramItem = {};
		std::string VertexModelItem   = {};

		std::string FrameBufferItem    = {};
		std::string FrameBufferTexItem = {};

		float     RenderTimer  = 0.0f;
		FsMatrix4 RenderMatrix = {};

		void calc_update_particles(std::vector<IFC_PARTC::ParticleAttributes>& ptc, float speed, float lifesub);
		void vertex_data_convert(const std::vector<IFC_PARTC::ParticleAttributes>& src, std::vector<float>& cvt);

	public:
		ImParticleFx(Vector2T<float> WindowResolution);
		~ImParticleFx();

		bool ParticleSystemUpdate();
		void ParticleCreate(IFC_PARTC::ParticleCreateBase* Generator);

		IFC_PARTC::ParticleSystemState				GetParticleState();
		uint32_t									GetParticleRenderHandle();
		std::vector<IFC_PARTC::ParticleAttributes>* GetParticleDataset();
	};
}

#endif