// improfx21_demo.

#ifndef _IMPROFX21_DEMO_H
#define _IMPROFX21_DEMO_H

#define ENABLE_EXTENSION
#include "../ImProFxUserHeader.h"

class ImprofxDemoClass21 :public CoreModuleIMFX::INTERFACE_PROFX_USER {
public:
	bool LogicInitialization(CoreModuleIMFX::FRMCORE_PACKAGE& FrmDat) override;
	bool LogicEventLoop(CoreModuleIMFX::FRMCORE_PACKAGE& FrmDat, CoreModuleIMFX::FRMCORE_INFO& FrmInfo) override;
	void LogicCloseFree() override;

	bool FixedWindowFlag = false;
	ImGuiWindowFlags WindowFlags = 0;

	uint32_t MenuTypeCount = 0;

	ImVec4 MenuConfigItemColor = ImVec4(0.55f, 0.12f, 1.0f, 0.72f);
	ImVec4 MenuConfigTextColor = ImVec4(0.0f, 1.0f, 0.85f, 0.92f);
	float MenuConfigTextSize = 1.2f;

	SmoothMenuChildWindow TestMenu;

	AnimNodesEditorWindow TestNE;

	CoordSystemEditorWindow TestCoord;

	float BgColor = 1.0f;
};

#endif