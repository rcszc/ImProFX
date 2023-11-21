// improfx20_test.

#ifndef _IMPROFX20_TEST_H
#define _IMPROFX20_TEST_H
#include "ImProFX2UserHeader.h"

class TestUserClass :public CoreModuleIMFX::INTERFACE_PROFX_USER {
protected:
	std::vector<uint32_t> ImProFX20page = {};

public:
	bool LogicInitialization(CoreModuleIMFX::FRMCORE_PACKAGE& FrmDat) override;
	bool LogicEventLoop(CoreModuleIMFX::FRMCORE_PACKAGE& FrmDat, CoreModuleIMFX::FRMCORE_INFO& FrmInfo) override;
	void LogicCloseFree() override;

	Vector2T<float> demo_page_xpos = {};
	Vector2T<bool>  demo_page_play = {};

	Vector2T<float> window_page_ypos = {};

	float slider_bg = 1.0f;
	Vector2T<float> slider_value[3] = { 
		Vector2T<float>(1.0f, 1.0f),
		Vector2T<float>(1.0f, 1.0f),
		Vector2T<float>(1.0f, 1.0f)
	};

	float multiline_anim_count = 0.0f;
};

#endif