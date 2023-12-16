// ImProFxUserRegister. [system_file]
#include "../imgui_profx_src/improfx_core/framework_core.hpp"

// user register class header_file:
#include "demo20/improfx20_demo.h"
#include "demo21/improfx21_demo.h"

// 1. 使用反射"FXUCLASS"注册类.
// 2. 使用"IMFXREF.ReflectionObjectCreate"创建注册对象.
// [2023_12_14] RCSZ.

//FXUCLASS(ImprofxDemoClass20);
FXUCLASS(ImprofxDemoClass21);

// [初始化阶段]: 用户类对象注册.
void CoreModuleSystem::ProfxSystemFramework::InitialStageRegUserClass()
{
	//IMFXREF.ReflectionObjectCreate("ImprofxDemoClass20", "ImProFx20Ademo");
	IMFXREF.ReflectionObjectCreate("ImprofxDemoClass21", "ImProFX21T");
}