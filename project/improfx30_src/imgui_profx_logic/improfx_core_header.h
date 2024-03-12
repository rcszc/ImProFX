// improfx_core_header.

#ifndef _IMPROFX_CORE_HEADER_H
#define _IMPROFX_CORE_HEADER_H
#include "../imgui_profx_src/improfx_core/framework_core.hpp"

namespace ProFxFramework {
	using namespace LOGCONS;        // 全局日志
	using namespace CoreModuleIMFX; // 核心模块
	using namespace ImProFxTools;   // 核心工具
}

namespace ProFxImGui {
	using namespace ImGuiPro;   // 扩展控件
	using namespace ImProFXgui; // 扩展模板控件
}

namespace ProFxModule {
	using namespace ImGuiProAnim; // 动画
	using namespace ImDynamicMem; // 内存
	using namespace ImParticle;   // 粒子
	using namespace ImThreadTask; // 线程池
}

// 启用框架扩展.
#ifdef ENABLE_EXTENSION
#define IMPROFX_EXTENSION_VER 200
#include "../imgui_profx_src/framework_extension/framework_extension_header.h"
#endif

#endif