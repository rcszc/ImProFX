// demo_header.

#ifndef _DEMO_HEADER_HPP
#define _DEMO_HEADER_HPP

#define ENABLE_EXTENSION
#include "ImProFxUserHeader.h"

class DemoHeaderClass :public CoreModuleIMFX::INTERFACE_PROFX_USER {
public:
	bool LogicInitialization(CoreModuleIMFX::FRMCORE_PACKAGE& FrmDat) override {

	}

	bool LogicEventLoop(CoreModuleIMFX::FRMCORE_PACKAGE& FrmDat, CoreModuleIMFX::FRMCORE_INFO& FrmInfo) override {

	}

	void LogicCloseFree() override {

	}
};

#endif