// improfx2_demo. RCSZ.
// 2023_12_05 version 2.0.0 alpha esay_demo.
// 窗口大小: 1680 x 945, 窗口大小固定: 是

#ifndef _IMPROFX2_DEMO_H
#define _IMPROFX2_DEMO_H

#define ENABLE_EXTENSION
#include "../ImProFxUserHeader.h"

class ImprofxDemoClass20 :public CoreModuleIMFX::INTERFACE_PROFX_USER {
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

	Vector2T<float> ParticleConfigLife     = Vector2T<float>(384.0f, 512.0f);
	Vector2T<float> ParticleConfigPosition = Vector2T<float>(-72.0f, 72.0f);
	Vector2T<float> ParticleConfigVector   = Vector2T<float>(-3.2f, 3.2f);
	Vector3T<bool>  ParticleConfigColorSys = {};

	float ParticleNumber = 256.0f;
	int ParticleMode = 1 << 2;

public:
	bool LogicInitialization(CoreModuleIMFX::FRMCORE_PACKAGE& FrmDat) override;
	bool LogicEventLoop(CoreModuleIMFX::FRMCORE_PACKAGE& FrmDat, CoreModuleIMFX::FRMCORE_INFO& FrmInfo) override;
	void LogicCloseFree() override;

	void ConfigAnimation(CoreModuleIMFX::FRMCORE_PACKAGE& FrmDat);

	void SecondaryWindow(CoreModuleIMFX::FRMCORE_PACKAGE& FrmDat, CoreModuleIMFX::FRMCORE_INFO& FrmInfo);
};

#endif