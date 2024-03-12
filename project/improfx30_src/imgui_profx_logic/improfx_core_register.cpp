// improfx_core_register.
#include "../imgui_profx_src/improfx_core/framework_core.hpp"

// register class header.
#include "../imgui_profx_logic/improfx_project_imps/src_editor/imps_editor.h"

FXUCLASS(IMPS_EditorMain);

void CoreModuleSystem::ProfxSystemFramework::InitialStageRegUserClass()
{
	IMFXREF.ReflectionObjectCreate("IMPS_EditorMain", "IMPS-EditorMain");
}