// ImProFxUserHeader. [system_header] RCSZ.
// ��ܵ���ͷ�ļ�.

#ifndef _IMPROFXUSERHEADER_H
#define _IMPROFXUSERHEADER_H
#include "../imgui_profx_src/improfx_core/framework_core.hpp"

namespace ProFxFramework {
	using namespace LOGCONS;        // ȫ����־
	using namespace CoreModuleIMFX; // ����ģ��
	using namespace ImProFxTools;   // ���Ĺ���
}

namespace ProFxImGui {
	using namespace ImGuiPro;   // ��չ�ؼ�
	using namespace ImGuiProRZ; // ��չģ��ؼ�
}

namespace ProFxModule {
	using namespace ImGuiProAnim; // ����
	using namespace ImDynamicMem; // �ڴ�
	using namespace ImParticle;   // ����
	using namespace ImThreadTask; // �̳߳�
}

// ���ÿ����չ.
#ifdef ENABLE_EXTENSION
#define IMPROFX_EXTENSION_VER 200
#include "../imgui_profx_src/framework_extension/framework_extension_header.h"
#endif

#endif