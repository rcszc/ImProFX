// framework_renderer. [2023_12_28] RCSZ
/*
* ImProFX 渲染抽象层: IMFXGL 
* 当前兼容: OpenGL 4.6 [64BIT] [C++17]
* Update: 20231228
*/

#ifndef _FRAMEWORK_RENDERER_H
#define _FRAMEWORK_RENDERER_H
#include "../framework_define.hpp"

#ifdef ENABLE_OPENGL_API
// opengl version 4.6 api.
#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/GLU.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#define DEFRES_FLAG_NORMAL   1 // 正常资源.
#define DEFRES_FLAG_INVALID -1 // 无效资源.

#define DEF_IMFXSTAT_SUCCESS 1
#define DEF_IMFXSTAT_FAILED  0

#define DEF_CHANNEL_RGB  3 // texture channels 3 bytes.
#define DEF_CHANNEL_RGBA 4 // texture channels 4 bytes.

using INIT_PARAMETERS = uint32_t;
using INIT_RETURN     = GLenum;

using ResUnique    = const std::string&;
using ResourceFlag = int;

using ImfxShader          = GLuint;
using ImfxTexture         = GLuint;
using ImfxVertexBuffer    = GLuint;
using ImfxVertexAttribute = GLuint;
using ImfxFrameBuffer     = GLuint;
using ImfxRenderBuffer    = GLuint;

constexpr GLuint OGLinvalidHandle = NULL;
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

// io save image_file mode_type.
enum SaveImageMode {

	ImageJPG = 1 << 1, // .jpg图像文件.
	ImagePNG = 1 << 2  // .png图像文件.
};

// renderer logger function object.
using RendererLogger = void (*)(const LOGLABEL&, const char*, const char*, ...);

#endif