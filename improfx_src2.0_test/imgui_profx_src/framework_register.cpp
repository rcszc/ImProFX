// framework_register.
#include "improfx_core/framework_core.hpp"

// user register class header file.
#include "../imgui_profx_logic/improfx20_test.h"

// 1. 使用反射"REGISTER_USER_CLASS"注册类.
// 2. 使用"IMFXOBJ.ReflectionObjectCreate"创建注册对象.
// [2023_11_16] RCSZ.

FXUCLASS(TestUserClass);

void RegUserClassFunction()
{
	IMFXREF.ReflectionObjectCreate("TestUserClass", "ImProFx20");
}