// framework_particle.

#ifndef _FRAMEWORK_PARTICLE_H
#define _FRAMEWORK_PARTICLE_H
#include "framework_particle_interface.h"

#define SYSMD_LB_PARTICLE "[IM_PARTICLE]: "

namespace ImParticle {

	class ImPTCgenerator :public IFC_PARTC::ParticleCreateBase {
	protected:
		std::vector<IFC_PARTC::ParticleAttributes> ParticlesGenCache = {};

		ParticleCreateMode CreateMode = {};
		size_t ParticlesNumber = 8;

		Vector3T<bool>     ColorSystemType = {};
		Vector2T<uint32_t> RandomLimitLife = {};

		Vector3T<float> OffsetPosition      = {};
		Vector2T<float> RandomLimitPosition = {}, RandomLimitVector = {};

	public:
		void ConfigCreateNumber(float number) override;
		void ConfigCreateMode(ParticleCreateMode mode) override;
		void ConfigLifeDispersion(Vector2T<float> rand_limit_life = Vector2T<float>(280.0f, 520.0f)) override;
		void ConfigRandomDispersion(
			// randomly distributed parameters.
			Vector2T<float> rand_limit_vector,   // vec random[min,max] pos += vec * speed.
			Vector2T<float> rand_limit_position, // pos random[min,max].
			Vector3T<bool>  color_system,        // color system(random) channels_switch.
			Vector3T<float> offset_position = Vector3T<float>()
		) override;

		void CreateAddParticleDataset(std::vector<IFC_PARTC::ParticleAttributes>& Data) override;
	};

	class ImParticleFx :public IFC_PARTC::ParticleBase, protected IMPROFX_LOW::IMPROFX_DATASET_LLRES {
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

		void CalcUpdateParticles(std::vector<IFC_PARTC::ParticleAttributes>& particles, float speed, float lifesub);
		void VertexDataConvert(const std::vector<IFC_PARTC::ParticleAttributes>& src, std::vector<float>& cvt);

	public:
		ImParticleFx(const Vector2T<float>& window_size);
		~ImParticleFx();

		void ParticleCreate(IFC_PARTC::ParticleCreateBase* generator) override;

		ImfxTexture									GetParticleRenderHandle() override;
		IFC_PARTC::ParticleSystemState				GetParticleState() override;
		std::vector<IFC_PARTC::ParticleAttributes>* GetParticleDataset() override;

		// framework system call.
		void _UPDATE_ParticleSystem() override;
	};
}

#endif