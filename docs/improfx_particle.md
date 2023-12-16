# ImProFx - Particle
- [x] Update.20231216 RCSZ

[返回使用文档](improfx_usage.md)
- 所在目录: src/imgui_profx_src/improfx_particle/

> 2023_12_02 version 2.0.0 Test-2
> 2023_12_08 version 2.0.0 Alpha
> 2023_12_16 version 2.1.0 Alpha

框架内粒子系统函数使用, 调用&创建&销毁 在corefx中的"ImProFxTools::FrmStatic"部分.

参考: [框架核心文档](improfx_corefx.md)

## 2.0.0A
> namespace ImParticle

粒子系统&粒子创建器类.
```cpp
class ImParticleFx :public IFC_PARTC::ParticleBase;
class ImPTCgenerator :public IFC_PARTC::ParticleCreateBase
```

### 粒子系统
> 渲染流程: 粒子数据 -> 数据集 -> GPU

粒子系统状态结构体.
```cpp
struct IFC_PARTC::ParticleSystemState {
	size_t DarwParticlesNumber; // 当前渲染粒子数量
	size_t DarwDatasetSize;     // 当前渲染数据集大小 bytes / sizeof(float)
};
```

粒子系统(ImParticleFx)成员函数.
```cpp
// 更新粒子系统, 在用户注册对象的"LogicEventLoop"使用
bool ParticleSystemUpdate();
// 创建粒子(效果)
// @param "Generator" (传入粒子创建类, 参考以下"粒子创建器")
void ParticleCreate(IFC_PARTC::ParticleCreateBase* Generator);

// 获取粒子系统实时状态结构体
IFC_PARTC::ParticleSystemState GetParticleState();
// 获取粒子系统渲染句柄(FBO附着纹理)
uint32_t GetParticleRenderHandle();
// 获取粒子数据指针(操作不当可能崩溃)
std::vector<IFC_PARTC::ParticleAttributes>* GetParticleDataset();
```

### 粒子创建器
> 创建流程: 数量 -> 扩散模式 -> 生命随机分布 -> 空间随机分布 -> 创建

粒子创建模式选择.
```cpp
enum ParticleCreateMode {
	PTCpoints   = 1 << 1, // 点     [扩散]
	PTCcircle   = 1 << 2, // 圆形   [扩散]
	PTCsquare   = 1 << 3, // 矩形   [扩散]
	PTCdrift    = 1 << 5  // 飘落   [效果]
};
```

粒子创建器配置.
```cpp
// @param "Number" (创建粒子数量)
void ConfigCreateNumber(float Number);

// @param "Mode" (创建模式, 参考以上模式选择)
void ConfigCreateMode(ParticleCreateMode Mode);

// @param "LifeRandLimit" (生命随机均匀分布限制)
// vector_x:min, vector_y:max
void ConfigLifeDispersion(Vector2T<float> LifeRandLimit = Vector2T<float>(280.0f, 520.0f));

// @param "VecRandLimit" (速度随机均匀分布限制)
// @param "PosRandLimit" (初速位置随机均匀分布限制)
// vector_x:min, vector_y:max

// @param "ColorSystem" (色系选择, 色系内随机颜色)
// vector_x:red, vector_y:green, vector_z:blue

// @param "OffsetPosition" (起始点偏移位置)
// vector_x:x, vector_y:y, vector_z:z
void ConfigRandomDispersion(
	Vector2T<float> VecRandLimit,
	Vector2T<float> PosRandLimit,
	Vector3T<bool>  ColorSystem,
	Vector3T<float> OffsetPosition = Vector3T<float>()
);

// 创建粒子数据(不需要手动调用)
void CreateAddParticleDataset(std::vector<IFC_PARTC::ParticleAttributes>& Data);
```

__粒子系统使用简单Demo:__
> 在框架内注册用户类中

```cpp
// 申请创建注册粒子系统对象:
FrmStatic.StaticParticleSysAlloc(FrmDat.ParticleSystem, "MyParticle", Vector2T<float>(1536.0f, 945.0f));

// 事件循环中获取调用粒子系统对象:
auto MyPtc = FrmStatic.StaticParticleSysFind(FrmDat.ParticleSystem, "MyParticle");
// 获取粒子系统状态:
auto State = MyPtc->GetParticleState();
// 更新粒子粒子系统:
MyPtc->ParticleSystemUpdate();

// 创建粒子效果:
if (MyFlag == true) {

    // 使用粒子创建器
    ImParticle::ImPTCgenerator MyPtcCreate;

    // 按照以上配置参数
    MyPtcCreate.ConfigNumber(Number);
    MyPtcCreate.ConfigLifeDispersion(ParamA);
    MyPtcCreate.ConfigCreateMode(ptcPoints);
    MyPtcCreate.ConfigRandomDispersion(ParamB, ParamC, ParamC);

    // 使用粒子系统, 传入粒子加载器
    MyPtc->ParticleCreate(&MyPtcCreate);
}
```
---

- 粒子系统使用的shader资源由自己管理分配释放, 与核心框架无关
- 目前开发重心不在此, 后几个版本可能不会有太多改变

```END```