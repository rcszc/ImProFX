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
			particles_number = (size_t)Number;
	}

	void ImPTCgenerator::ConfigCreateMode(ParticleCreateMode Mode) {
		create_mode = Mode;
	}

	void ImPTCgenerator::ConfigLifeDispersion(Vector2T<float> LifeRandLimit) {
		// limit > 0.0f, 0.0f:free_particle.
		LifeRandLimit.vector_x = LifeRandLimit.vector_x <= 0.0f ? 0.0f : LifeRandLimit.vector_x;
		LifeRandLimit.vector_y = LifeRandLimit.vector_y <= 0.0f ? 0.0f : LifeRandLimit.vector_y;
		randlife_limit = Vector2T<uint32_t>((uint32_t)LifeRandLimit.vector_x, (uint32_t)LifeRandLimit.vector_y);
	}

	void ImPTCgenerator::ConfigRandomDispersion(Vector2T<float> VecRandLimit, Vector2T<float> PosRandLimit, Vector3T<bool>  ColorSystem, Vector3T<float> OffsetPosition) {
		randvec_limit = VecRandLimit; randpos_limit = PosRandLimit;
		rand_colorsystem = ColorSystem; position_offset = OffsetPosition;
	}

	void ImPTCgenerator::CreateAddParticleDataset(vector<ParticleAttributes>& Data) {
		switch (create_mode) {
		// "VecRandLimit" x,y [speed]
		case(ptcPoints): {
			for (size_t i = 0; i < particles_number; ++i) {
				ParticleAttributes temp_particle = {};

				particle_disp_pos(temp_particle, randpos_limit, position_offset);
				particle_disp_vec(temp_particle, randvec_limit);
				particle_colorsys(temp_particle, rand_colorsystem);

				temp_particle.ParticleLife = random_timeseed_fp32((float)randlife_limit.vector_x, (float)randlife_limit.vector_y);
				temp_paricles.push_back(temp_particle);
			}
			break;
		}
		// "VecRandLimit" x:cricle_x_scale, y:cricle_y_scale [speed]
		case(ptcCircle): {
			for (size_t i = 0; i < particles_number; ++i) {
				ParticleAttributes temp_particle = {};

				particle_disp_pos(temp_particle, randpos_limit, position_offset);
				particle_colorsys(temp_particle, rand_colorsystem);

				temp_particle.ParticleVector = 
					Vector3T<float>(PTCMS_SIN(i * 3.8f) * randvec_limit.vector_x, PTCMS_COS(i * 3.8f) * randvec_limit.vector_y, 0.0f);

				temp_particle.ParticleLife = random_timeseed_fp32((float)randlife_limit.vector_x, (float)randlife_limit.vector_y);
				temp_paricles.push_back(temp_particle);
			}
			break;
		}
		// "VecRandLimit" x:square_side, y:null [speed] 
		case(ptcSquare): {
			const float center_x = 0.0f, center_y = 0.0f;
			vector<Vector3T<float>> smp_point = {};

			float side_length = randvec_limit.vector_x;
			float smp_step = side_length / (float)particles_number * 4.0f;

			for (int i = 0; i < particles_number / 4; ++i) {
				smp_point.push_back(Vector3T<float>(-(side_length / 2.0f) + (float)i * smp_step, -(side_length / 2.0f), 0.0f));
				smp_point.push_back(Vector3T<float>(side_length / 2.0f, -(side_length / 2.0f) + (float)i * smp_step, 0.0f));
				smp_point.push_back(Vector3T<float>(-(side_length / 2.0f) + (float)i * smp_step, side_length / 2.0f, 0.0f));
				smp_point.push_back(Vector3T<float>(-(side_length / 2.0f), -(side_length / 2.0f) + (float)i * smp_step, 0.0f));
			}

			for (const auto& item : smp_point) {
				ParticleAttributes temp_particle = {};

				particle_disp_pos(temp_particle, randpos_limit, position_offset);
				particle_colorsys(temp_particle, rand_colorsystem);

				temp_particle.ParticleVector = item;

				temp_particle.ParticleLife = random_timeseed_fp32((float)randlife_limit.vector_x, (float)randlife_limit.vector_y);
				temp_paricles.push_back(temp_particle);
			}
			break;
		}
		// "VecRandLimit" x:abs(y_scale), y [speed]
		// "OffsetPosition" x, y:high(offset)
		case(ptcDrift): {
			for (size_t i = 0; i < particles_number; ++i) {
				ParticleAttributes temp_particle = {};

				particle_colorsys(temp_particle, rand_colorsystem);

				temp_particle.ParticlePosition.vector_y = random_timeseed_fp32(-700.0f, -300.0f) + position_offset.vector_y;
				temp_particle.ParticlePosition.vector_x = random_timeseed_fp32(randpos_limit.vector_x, randpos_limit.vector_y) + position_offset.vector_x;
				temp_particle.ParticlePosition.vector_z = random_timeseed_fp32(randpos_limit.vector_x, randpos_limit.vector_y) + position_offset.vector_z;

				particle_disp_vec(temp_particle, randvec_limit);
				temp_particle.ParticleVector.vector_y = abs(randvec_limit.vector_x);

				temp_particle.ParticleLife = random_timeseed_fp32((float)randlife_limit.vector_x, (float)randlife_limit.vector_y);
				temp_paricles.push_back(temp_particle);
			}
			break;
		}
		}
		Data.insert(Data.end(), temp_paricles.begin(), temp_paricles.end());
		PushLogger(LOG_INFO, SYSMD_LB_PARTICLE, "create particles: %u", temp_paricles.size());
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
		Vector3T<float> tmp_position = {};
		Vector4T<float> tmp_color    = {};

		cvt.clear();
		for (const auto& item : src) {

			tmp_position = item.ParticlePosition;
			tmp_color = item.ParticleColor;

			float particle_default[72] = {
				-5.0f + tmp_position.vector_x, -5.0f + tmp_position.vector_y, 0.0f + tmp_position.vector_z,
				 0.0f + tmp_color.vector_x, 0.0f + tmp_color.vector_y, 0.0f + tmp_color.vector_z, 0.0f + tmp_color.vector_w, 0.0f, 0.0f, 0.0f,0.0f,0.0f,
				 5.0f + tmp_position.vector_x, -5.0f + tmp_position.vector_y, 0.0f + tmp_position.vector_z,
				 0.0f + tmp_color.vector_x, 0.0f + tmp_color.vector_y, 0.0f + tmp_color.vector_z, 0.0f + tmp_color.vector_w, 1.0f, 0.0f, 0.0f,0.0f,0.0f,
				 5.0f + tmp_position.vector_x,  5.0f + tmp_position.vector_y, 0.0f + tmp_position.vector_z,
				 0.0f + tmp_color.vector_x, 0.0f + tmp_color.vector_y, 0.0f + tmp_color.vector_z, 0.0f + tmp_color.vector_w, 1.0f, 1.0f, 0.0f,0.0f,0.0f,

				-5.0f + tmp_position.vector_x, -5.0f + tmp_position.vector_y, 0.0f + tmp_position.vector_z,
				 0.0f + tmp_color.vector_x, 0.0f + tmp_color.vector_y, 0.0f + tmp_color.vector_z, 0.0f + tmp_color.vector_w, 0.0f, 0.0f, 0.0f,0.0f,0.0f,
				 5.0f + tmp_position.vector_x,  5.0f + tmp_position.vector_y, 0.0f + tmp_position.vector_z,
				 0.0f + tmp_color.vector_x, 0.0f + tmp_color.vector_y, 0.0f + tmp_color.vector_z, 0.0f + tmp_color.vector_w, 1.0f, 1.0f, 0.0f,0.0f,0.0f,
				-5.0f + tmp_position.vector_x,  5.0f + tmp_position.vector_y, 0.0f + tmp_position.vector_z,
				 0.0f + tmp_color.vector_x, 0.0f + tmp_color.vector_y, 0.0f + tmp_color.vector_z, 0.0f + tmp_color.vector_w, 0.0f, 1.0f, 0.0f,0.0f,0.0f
			};
			// src => particle vert => cvt
			cvt.insert(cvt.end(), particle_default, particle_default + ParticleVertLen);
		}
	}

	ImParticleFx::ImParticleFx(Vector2T<float> WindowResolution) {
		FSloadShader particle_shader_script;

		particle_shader_script.load_shader_vs(particle_shader_script::FxShaderDefaultVert, IFC_SHADER::StringScript);
		particle_shader_script.load_shader_fs(particle_shader_script::FxShaderDefaultFrag, IFC_SHADER::StringScript);

		shader_program_handle = particle_shader_script.create_program_handle();
		particle_shader_script.delete_shader();

		particle_model_handle.vbo_handle = SFglobalTransmitData.create_vbo_handle();
		particle_model_handle.vao_handle = SFglobalTransmitData.create_vao_handle();

		float res_scale = WindowResolution.vector_x / WindowResolution.vector_y;

		glm::mat4 proj_matrix = glm::ortho(-matrix_space * res_scale, matrix_space * res_scale, -matrix_space, matrix_space, -matrix_space, matrix_space);
		glm::mat4 temp_mvp_matrix = proj_matrix; // * view_matrix

		const float* glmmatptr = glm::value_ptr(temp_mvp_matrix); // get glm matrix ptr.
		memcpy_s(render_matrix_mvp.matrix, 16 * sizeof(float), glmmatptr, 16 * sizeof(float));

		// render => fbo => texture.
		FScreateTexture create_texture(TextureColorRGBA, WindowResolution);

		FSloadFrameBuffer create_fbo;
		create_fbo.create_bind_framebuffer(create_texture.get_texture_handle());

		framebuffer_texture = FsTextureHandle(create_texture.get_texture_handle(), shader_program_handle, NULL, "PTC");
		framebuffer_handle = create_fbo.get_framebuffer_handle();

		create_texture.unbind_texture();

		PushLogger(LOG_TRC, SYSMD_LB_PARTICLE, "particle system init.");
		PushLogger(LOG_INFO, SYSMD_LB_PARTICLE, "particle system render_resolution: %.2f x %.2f", WindowResolution.vector_x, WindowResolution.vector_y);
	}

	ImParticleFx::~ImParticleFx() {
		// delete opengl handle.
		SFglobalDeletel.delete_texture_handle(framebuffer_texture);
		SFglobalDeletel.delete_model_handle(particle_model_handle);
		SFglobalDeletel.delete_framebuffer_handle(framebuffer_handle);
		SFglobalDeletel.delete_program_handle(shader_program_handle);

		PushLogger(LOG_TRC, SYSMD_LB_PARTICLE, "particle system free.");
	}

	bool ImParticleFx::ParticleSystemUpdate() {
		// calculate particle state.
		calc_update_particles(data_particles, 0.5f, 1.0f);
		// "particle_attributes" => float
		vertex_data_convert(data_particles, data_vertices);
		// convert => update vert.
		SFglobalTransmitData.update_vertex_data(FsVertexGroups(data_vertices.size(), data_vertices.data()), particle_model_handle);

		FS_FrameBufferAbove(framebuffer_handle);
		FS_ClearFrameBuffer();

		FS_ShaderEnable(shader_program_handle);
		// draw particles vert.
		FS_ShaderDrawMD(particle_model_handle, int(data_vertices.size() / ParticleVertAtt));

		// system param.
		SFglobalUniform.uniform_matrix4x4(shader_program_handle, "FxMatMvp", render_matrix_mvp);
		SFglobalUniform.uniform_float(shader_program_handle, "FxTime", render_timer);

		FS_ShaderEnable(NULL);
		FS_FrameBufferBelow();
		FS_ClearFrameBuffer();

		render_timer += RENDERLOOP_TIMER_STEP;
		return true;
	}

	ParticleSystemState ImParticleFx::GetParticleState() {
		ParticleSystemState ResultState = {};

		ResultState.DarwParticlesNumber = data_particles.size();
		ResultState.DarwDatasetSize = data_vertices.size();

		return ResultState;
	}

	void ImParticleFx::ParticleCreate(ParticleCreateBase* Generator) {
		Generator->CreateAddParticleDataset(data_particles);
	}

	uint32_t ImParticleFx::GetParticleRenderHandle() { 
		return framebuffer_texture.texture_hd; 
	}

	vector<ParticleAttributes>* ImParticleFx::GetParticleDataset() {
		return &data_particles;
	}
}