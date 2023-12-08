// framework_particle.

#ifndef _FRAMEWORK_PARTICLE_H
#define _FRAMEWORK_PARTICLE_H
#include "framework_particle_interface.h"

#define SYSMD_LB_PARTICLE "[im_particle]: "

namespace particle_shader_script {
	extern const char* FxShaderDefaultVert;
	extern const char* FxShaderDefaultFrag;
}

namespace ImParticle {

	class ImPTCgenerator :public IFC_PARTC::ParticleCreateBase {
	protected:
		std::vector<IFC_PARTC::ParticleAttributes> temp_paricles = {};

		size_t particles_number = 8;
		ParticleCreateMode create_mode = {};

		Vector3T<bool>     rand_colorsystem = {};
		Vector2T<uint32_t> randlife_limit   = {};

		Vector3T<float> position_offset = {};
		Vector2T<float> randpos_limit = {}, randvec_limit = {};

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

	class ImParticleFx :public IFC_PARTC::ParticleBase {
	protected:
		const float matrix_space = 384.0f;
		std::vector<IFC_PARTC::ParticleAttributes> data_particles = {};
		std::vector<float>                          data_vertices  = {};

		FsModelHandle particle_model_handle = {};

		uint32_t shader_program_handle = NULL;
		uint32_t framebuffer_handle    = NULL;

		FsTextureHandle framebuffer_texture = {};

		float     render_timer      = 0.0f;
		FsMatrix4 render_matrix_mvp = {};

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