// imgui-profx.

#include <iostream>
#include "imgui_profx_src/improfx_core/framework_render.h"

using namespace std;

#pragma warning(disable: 4819) // (无丝竹之乱耳

int main() {

	CoreModuleRender::FrameworkRender* frameobj = new CoreModuleRender::FrameworkRender();
	CoreModuleSystem::ProfxSystemFramework _START(frameobj);

	return _START.StartProfx();
}