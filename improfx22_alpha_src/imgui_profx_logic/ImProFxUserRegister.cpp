// ImProFxUserRegister. [system_file] RCSZ.
#include "../imgui_profx_src/improfx_core/framework_core.hpp"

// user register class header_file:
#include "demo2/improfx2_demo.h"

// 1. 使用反射"FXUCLASS"注册类.
// 2. 使用"IMFXREF.ReflectionObjectCreate"创建注册对象.
// [2023_12_14] RCSZ.

FXUCLASS(ImprofxDemoClass22);

// [初始化阶段]: 用户类对象注册.
void CoreModuleSystem::ProfxSystemFramework::InitialStageRegUserClass()
{
	IMFXREF.ReflectionObjectCreate("ImprofxDemoClass22", "ImProFx20Ademo");
}