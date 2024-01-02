// framework_renderer. [2023_12_28] RCSZ
// ImProFX 渲染抽象层: IMFXGL 
// 当前兼容: OpenGL 4.6 [64BIT] [C++17]

// Update: 20231228

#ifndef _FRAMEWORK_RENDERER_H
#define _FRAMEWORK_RENDERER_H
#include "../framework_define.hpp"

#define ENABLE_OPENGL_API

#ifdef ENABLE_OPENGL_API
#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/GLU.h>
// opengl 4.6 api handle.

#define DEFRES_FLAG_NORMAL   1 // 正常资源.
#define DEFRES_FLAG_EMPTY    0 // 空资源.
#define DEFRES_FLAG_INVALID -1 // 无效资源.

#define DEF_IMFXSTAT_SUCCESS 1
#define DEF_IMFXSTAT_FAILED  0

#define DEF_CHANNEL_RGB  3 // texture channels 3 bytes.
#define DEF_CHANNEL_RGBA 4 // texture channels 4 bytes.

using INIT_PARAMETERS = uint32_t;
using INIT_RETURN     = GLenum;

using ResUnique    = const std::string&;
using ResourceFlag = int;

using ImfxShader          = uint32_t;
using ImfxTexture         = uint32_t;
using ImfxVertexBuffer    = uint32_t;
using ImfxVertexAttribute = uint32_t;
using ImfxFrameBuffer     = uint32_t;
using ImfxRenderBuffer    = uint32_t;
#endif

// shader read script mode. 
enum ScriptReadMode {

	StringFilepath = 1 << 1, // 着色器文件路径.
	StringScript   = 1 << 2  // 着色器文本.
};

// shader sampling texture filter mode.
enum TextureFilterMode {

	NearestNeighborFiltering = 1 << 1, // 邻近过滤.
	LinearFiltering          = 1 << 2, // 线性过滤.
	AnisotropicFiltering     = 1 << 3, // 各向异性过滤.
	MipmapFiltering          = 1 << 4  // Mipmap过滤.
};

// renderer logger function object.
using RendererLogger = void (*)(const LOGLABEL&, const char*, const char*, ...);

#endif