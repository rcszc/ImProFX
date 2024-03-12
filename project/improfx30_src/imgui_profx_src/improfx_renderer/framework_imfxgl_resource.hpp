// framework_imfxgl_resource. 2024.01.01 RCSZ.
// LLRES(lowlevel resource)

#ifndef _FRAMEWORK_IMFXGL_RESOURCE_H
#define _FRAMEWORK_IMFXGL_RESOURCE_H
#include "low_graphics_opengl/improfx_opengl.hpp"
// low_graphics_xxx => framework_imfxgl_resource => framework_core

#define IMFX_LLRES_LABEL "[IMFX_LLRES]: "
#define IMFX_LLRES_DELETE(PTR) delete PTR; PTR = nullptr;

namespace IMPROFX_LOW {
	// improfx framework lowlevel graphic dataset.
	class IMPROFX_DATASET_LLRES {
	private:
		static RendererLogger LLRESLoggerFunction;

		template<typename T>
		bool LLRES_RES_FREE(RendererLogger LFUNC, T* POINTER, const char* LABEL) {
			if (POINTER == nullptr) {
				LFUNC(LOG_ERROR, IMFX_LLRES_LABEL, "LLRES FREE_RES %s = NULLPTR.", LABEL);
				return DEF_IMFXSTAT_FAILED;
			}
			else {
				IMFX_LLRES_DELETE(POINTER)
				return DEF_IMFXSTAT_SUCCESS;
			}
		}
	protected:
		// resource data(lowlevel_resource).
		static IMFX_OGL_RES::ImFXOGLresTexSampler*   LLRES_Samplers;
		static IMFX_OGL_RES::ImFXOGLresShader*       LLRES_Shaders;
		static IMFX_OGL_RES::ImFXOGLresTexture*      LLRES_Textures;
		static IMFX_OGL_RES::ImFXOGLresVertexBuffer* LLRES_VertexBuffers;
		static IMFX_OGL_RES::ImFXOGLresVertexAttr*   LLRES_VertexAttributes;
		static IMFX_OGL_RES::ImFXOGLresFrameBuffer*  LLRES_FrameBuffers;
		static IMFX_OGL_RES::ImFXOGLresRenderBuffer* LLRES_RenderBuffers;

		static size_t LLRES_GET_ALLSIZE();

		void LowLevelResourceCreate(RendererLogger LogFunc, uint32_t ResTMU = 32);
		bool LowLevelResourceFree();
	};
}

#endif