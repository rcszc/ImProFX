// framework_particle_interface.
// [INTERFACE].

#ifndef _FRAMEWORK_PARTICLE_INTERFACE_H
#define _FRAMEWORK_PARTICLE_INTERFACE_H
#include "../improfx_log/framework_log.hpp"

enum ParticleCreateMode {
	ptcPoints   = 1 << 1, // 点.
	ptcCircle   = 1 << 2, // 圆形 [扩散]
	ptcSquare   = 1 << 3, // 矩形 [扩散]
	ptcTriangle = 1 << 4, // 三角形 [扩散]
	ptcDrift    = 1 << 5  // 飘落 [效果]
};

// [2023_11_26](1): ImProFx框架粒子系统接口.
namespace IFC_PARTC {

	enum ParticleMode {
		particle_static  = 2, // no_update vector,position.
		particle_dynamic = 4  // update vector,position,color.
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
			ParticleModeType (particle_dynamic)
		{}
	};
	
	// 创建例子效果(使用[均匀分布]随机数模型).
	class ParticleCreateBase {
	public:
		// particle generation number[n > 8].
		virtual void ConfigCreateNumber(float Number) = 0;
		// 运算模式: "ParticleCreateMode"
		virtual void ConfigCreateMode(ParticleCreateMode Mode) = 0;
		// 粒子生命: life.xy life_timer[min,max]. 
		virtual void ConfigLifeDispersion(Vector2T<float> LifeRandLimit) = 0;

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
