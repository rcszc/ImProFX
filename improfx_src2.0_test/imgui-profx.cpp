﻿/* 
* ImGuiProFX - II, Version 2.0.0 TEST @RCSZ 2023 - 2024.
* Libray: OpenGL GLFW, OpenGL GLEW, ImGui, RapidJSON, stb_iamge.
* Msvc C++17 Release x64, IDE: Visual Studio 2022.
* --------------------------------------------------------
* 基于ImGui和OpenGL着色器的GUI框架, 为提升原版ImGui的视角效果以及体验.
* 为开发者提供更多工具, 保留ImGui的即时性和简洁.
*/

#include <iostream>
#include "imgui_profx_src/improfx_core/framework_core.hpp"

using namespace std;

#pragma warning(disable: 4819) // (无丝竹之乱耳

int main() {
	
	CoreModuleIMFX::FrameworkImCore* framework2object = new CoreModuleIMFX::FrameworkImCore();
	IMFX_SYSTEM_OBJ.FrameworkRegStart(framework2object);
	RegUserClassFunction();
	
	return IMFX_SYSTEM_OBJ.StartProfx();
}
