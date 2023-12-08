// improfx2_demo. 2023_12_05.

#ifndef _IMPROFX2_DEMO_H
#define _IMPROFX2_DEMO_H
#define ENABLE_EXTENSION
#include "../ImProFX2UserHeader.h"

class ImprofxDemoClass :public CoreModuleIMFX::INTERFACE_PROFX_USER {
protected:
	std::vector<uint32_t> SecWindowIcon = {};
	Vector3T<bool> SecWindowFlags = {};

	std::vector<uint32_t> DemoMainPage = {};
	std::vector<uint32_t> DemoMainPageIcon = {};

	Vector2T<float> WindowPageYpos = {};
	Vector2T<float> NextPageYpos = {};

	Vector2T<bool> DemoMainPagePlayflags = {};
	Vector2T<float> DemoMainPageXpos = {};

	Vector2T<float> SettingsBgColor = Vector2T<float>(1.0f, 1.0f);
	Vector2T<float> SettingsBgSpeed = Vector2T<float>(1.0f, 1.0f);

	float AboutTextAnim = 0.0f;
	uint32_t ArchImage = NULL;

	Vector2T<float> PTCcfgLife = Vector2T<float>(384.0f, 512.0f);
	Vector2T<float> PTCcfgPosition = Vector2T<float>(-72.0f, 72.0f);
	Vector2T<float> PTCcfgVector = Vector2T<float>(-3.2f, 3.2f);
	Vector3T<bool> PTCcfgColorSys = {};

	float ParticleNumber = 256.0f;
	int ParticleMode = 1 << 2;

public:
	bool LogicInitialization(CoreModuleIMFX::FRMCORE_PACKAGE& FrmDat) override;
	bool LogicEventLoop(CoreModuleIMFX::FRMCORE_PACKAGE& FrmDat, CoreModuleIMFX::FRMCORE_INFO& FrmInfo) override;
	void LogicCloseFree() override;

	void ConfigAnimation(CoreModuleIMFX::FRMCORE_PACKAGE& FrmDat);

	void SecondaryWindow(CoreModuleIMFX::FRMCORE_PACKAGE& FrmDat);
};

#endif