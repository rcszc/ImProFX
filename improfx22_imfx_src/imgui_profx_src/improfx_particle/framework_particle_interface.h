// framework_particle_interface.
// [INTERFACE].

#ifndef _FRAMEWORK_PARTICLE_INTERFACE_H
#define _FRAMEWORK_PARTICLE_INTERFACE_H
#include "../improfx_log/framework_log.hpp"
#include "../improfx_renderer/framework_imfxgl_resource.h"

enum ParticleCreateMode {
	PTCpoints   = 1 << 1, // 点.
	PTCcircle   = 1 << 2, // 圆形 [扩散]
	PTCsquare   = 1 << 3, // 矩形 [扩散]
	PTCdrift    = 1 << 4  // 飘落 [效果]
};

// [2023_11_26](1): ImProFx框架粒子系统接口.
namespace IFC_PARTC {

	enum ParticleMode {
		ParticleStatic  = 2, // no_update vector,position.
		ParticleDynamic = 4  // update vector,position,color.
	};

	struct ParticleAttributes {
		Vector3T<float> ParticleVector;
		Vector3T<float> ParticlePosition;
		Vector4T<float> ParticleColor;

		float ParticleLife;
		ParticleMode ParticleModeType;

		constexpr ParticleAttributes() :
			ParticleVector   ({}),
			ParticlePosition ({}),
			ParticleColor    ({}),
			ParticleLife     (0.0f),
			ParticleModeType (ParticleDynamic)
		{}
	};
	
	// 创建例子效果(使用[均匀分布]随机数模型).
	class ParticleCreateBase {
	public:
		// particle generation number[n > 8].
		virtual void ConfigCreateNumber(float Number) = 0;
		
		virtual void ConfigCreateMode(ParticleCreateMode Mode) = 0;           // 运算模式.
		virtual void ConfigLifeDispersion(Vector2T<float> LifeRandLimit) = 0; // 随机生命.

		// 粒子空间: vector.xy scale[min,max], position.xy scale[min,max], color_system.xyz switch[rd,rd,rd].
		// "OffsetPosition" 创建偏移位置(中心偏移).
		virtual void ConfigRandomDispersion(
			Vector2T<float> VecRandLimit,
			Vector2T<float> PosRandLimit,
			Vector3T<bool>  ColorSystem,
			Vector3T<float> OffsetPosition
		) = 0;

		virtual void CreateAddParticleDataset(std::vector<ParticleAttributes>& Data) = 0;
	};

	struct ParticleSystemState {
		size_t DarwParticlesNumber;
		size_t DarwDatasetSize;
	};

	// particle shader 由粒子系统自己管理(与框架核心渲染包无关).
	class ParticleBase {
	public:
		virtual bool ParticleSystemUpdate() = 0;
		virtual void ParticleCreate(ParticleCreateBase* Generator) = 0;

		// get particle system state.
		virtual ParticleSystemState GetParticleState() = 0;
		// get particle render texture handle.
		virtual uint32_t GetParticleRenderHandle() = 0;
		// get particle dataset(vector) pointer.
		virtual std::vector<ParticleAttributes>* GetParticleDataset() = 0;
		virtual ~ParticleBase() = default;
	};
}

#endif
