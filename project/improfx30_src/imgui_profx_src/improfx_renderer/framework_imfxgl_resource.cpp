// framework_imfxgl_resource.
#include "framework_imfxgl_resource.hpp"

namespace IMPROFX_LOW {
	IMFX_OGL_RES::ImFXOGLresTexSampler*   IMPROFX_DATASET_LLRES::LLRES_Samplers         = nullptr;
	IMFX_OGL_RES::ImFXOGLresShader*       IMPROFX_DATASET_LLRES::LLRES_Shaders          = {};
	IMFX_OGL_RES::ImFXOGLresTexture*      IMPROFX_DATASET_LLRES::LLRES_Textures         = {};
	IMFX_OGL_RES::ImFXOGLresVertexBuffer* IMPROFX_DATASET_LLRES::LLRES_VertexBuffers    = {};
	IMFX_OGL_RES::ImFXOGLresVertexAttr*   IMPROFX_DATASET_LLRES::LLRES_VertexAttributes = {};
	IMFX_OGL_RES::ImFXOGLresFrameBuffer*  IMPROFX_DATASET_LLRES::LLRES_FrameBuffers     = {};
	IMFX_OGL_RES::ImFXOGLresRenderBuffer* IMPROFX_DATASET_LLRES::LLRES_RenderBuffers    = {};

	RendererLogger IMPROFX_DATASET_LLRES::LLRESLoggerFunction = {};

	size_t IMPROFX_DATASET_LLRES::LLRES_GET_ALLSIZE() {
		// 不包含纹理采样器(TMU)资源.
		return LLRES_Shaders->ResourceSize() + LLRES_Textures->ResourceSize() + LLRES_VertexBuffers->ResourceSize() +
			LLRES_VertexAttributes->ResourceSize() + LLRES_FrameBuffers->ResourceSize() + LLRES_RenderBuffers->ResourceSize();
	}

	void IMPROFX_DATASET_LLRES::LowLevelResourceCreate(RendererLogger LogFunc, uint32_t ResTMU) {
		// LLRES CREATE SINGLE.
		LLRES_Samplers         = new IMFX_OGL_RES::ImFXOGLresTexSampler(ResTMU);
		LLRES_Shaders          = new IMFX_OGL_RES::ImFXOGLresShader();
		LLRES_Textures         = new IMFX_OGL_RES::ImFXOGLresTexture();
		LLRES_VertexBuffers    = new IMFX_OGL_RES::ImFXOGLresVertexBuffer();
		LLRES_VertexAttributes = new IMFX_OGL_RES::ImFXOGLresVertexAttr();
		LLRES_FrameBuffers     = new IMFX_OGL_RES::ImFXOGLresFrameBuffer();
		LLRES_RenderBuffers    = new IMFX_OGL_RES::ImFXOGLresRenderBuffer();
		// LLRES LOGGER FUNCTION GET.
		LLRESLoggerFunction = LogFunc;
	}

	bool IMPROFX_DATASET_LLRES::LowLevelResourceFree() {
		bool ReturnFlag = false;
		ReturnFlag |= !LLRES_RES_FREE(LLRESLoggerFunction, LLRES_Samplers,         "TEX_SAMPLER");
		ReturnFlag |= !LLRES_RES_FREE(LLRESLoggerFunction, LLRES_Shaders,          "SHADER");
		ReturnFlag |= !LLRES_RES_FREE(LLRESLoggerFunction, LLRES_Textures,         "TEXTURE");
		ReturnFlag |= !LLRES_RES_FREE(LLRESLoggerFunction, LLRES_VertexBuffers,    "VER_BUFFER");
		ReturnFlag |= !LLRES_RES_FREE(LLRESLoggerFunction, LLRES_VertexAttributes, "VER_ATTRI");
		ReturnFlag |= !LLRES_RES_FREE(LLRESLoggerFunction, LLRES_FrameBuffers,     "FRAME_BUFFER");
		ReturnFlag |= !LLRES_RES_FREE(LLRESLoggerFunction, LLRES_RenderBuffers,    "RENDER_BUFFER");
		return !ReturnFlag;
	}
}