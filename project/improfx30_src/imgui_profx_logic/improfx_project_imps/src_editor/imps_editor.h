// imps_editor.

#ifndef _IMPS_EDITOR_H
#define _IMPS_EDITOR_H

#define ENABLE_EXTENSION
#include "../../improfx_core_header.h"

#define LOGIC_LB_IMPS_EDITOR "[IMPS_EDITOR]:"

class IMPS_EditorMain :public CoreModuleIMFX::INTERFACE_PROFX_USER, public IMPROFX_LOW::IMPROFX_DATASET_LLRES {
public:
	bool LogicInitialization(CoreModuleIMFX::FRMCORE_PACKAGE& FrmDat) override;
	bool LogicEventLoop(CoreModuleIMFX::FRMCORE_PACKAGE& FrmDat, CoreModuleIMFX::FRMCORE_INFO& FrmInfo) override;
	void LogicCloseFree() override {};

	IMFXC_CWIN::DashboardChildWindow DevTest;

	float DevTestValue = 0.0f;

	ImVec2 SetLimit = ImVec2(0.0f, 400.0f);
};

#endif