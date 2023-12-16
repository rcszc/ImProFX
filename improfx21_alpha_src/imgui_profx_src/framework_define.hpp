// framework_define.

#ifndef _FRAMEWORK_DEFINE_HPP
#define _FRAMEWORK_DEFINE_HPP
#include <string>
#include <vector>
#include <queue>
#include <unordered_map>

#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>

#include <fstream>
#include <filesystem>

#include <functional>
#include <typeinfo> // RTTI.

// warring old_function.
#if defined(_MSC_VER) // MSVC.
#define OLD_FUNCTION __declspec(deprecated)
#else                 // GCC.
#define OLD_FUNCTION __attribute__((deprecated))
#endif

#define DEF_CORE_SYSTEM_CONFIG  "imgui_profx_system/config/window_config.json"
#define DEF_INIT_THREAD_WORKERS 32

#define STAGE_EXIT_INIT -2
#define STAGE_EXIT_FREE -3
#define STAGE_EXIT_LOOP -4

#define RENDERLOOP_TIMER_STEP 0.025f

#define CVT_IMPTR (void*)(intptr_t)

template <typename mvec>
// vector 2d x, y.
struct Vector2T {
	mvec vector_x, vector_y;
	constexpr Vector2T() : vector_x(0), vector_y(0) {}
	constexpr Vector2T(mvec x, mvec y) : vector_x(x), vector_y(y) {}

	mvec* data() { return &vector_x; }
	const mvec* data() const { return &vector_x; }
};

template <typename mvec>
// vector 3d x, y, z.
struct Vector3T {
	mvec vector_x, vector_y, vector_z;
	constexpr Vector3T() : vector_x(0), vector_y(0), vector_z(0) {}
	constexpr Vector3T(mvec x, mvec y, mvec z) : vector_x(x), vector_y(y), vector_z(z) {}

	mvec* data() { return &vector_x; }
	const mvec* data() const { return &vector_x; }
};

template <typename mvec>
// vector 4d x, y, z, w.
struct Vector4T {
	mvec vector_x, vector_y, vector_z, vector_w;
	constexpr Vector4T() : vector_x(0), vector_y(0), vector_z(0), vector_w(0) {}
	constexpr Vector4T(mvec x, mvec y, mvec z, mvec w) : vector_x(x), vector_y(y), vector_z(z), vector_w(w) {}

	mvec* data() { return &vector_x; }
	const mvec* data() const { return &vector_x; }
};

#define FS_ELEMENT_VERTEX 3 // update[2023_11_27] 四边形图元 => 三角形图元.
#define FS_VERTEX_COORD   3
#define FS_VERTEX_COLOR   4
#define FS_VERTEX_TEXUV   2

// vertex buffer object, vertex array object.
struct FsModelHandle {
	uint32_t VaoHandle, VboHandle;

	constexpr FsModelHandle() : VaoHandle(0), VboHandle(0) {}
	constexpr FsModelHandle(const uint32_t& vao, const uint32_t& vbo) :
		VaoHandle (vao), 
		VboHandle (vbo)
	{}
};

// texture handle group. [texture2d]
struct FsTextureHandle {
	uint32_t    TextureHandle, ProgramHandle;
	uint32_t    SamplerCount;
	std::string UniformName;

	FsTextureHandle() : TextureHandle(0), ProgramHandle(0), SamplerCount(0), UniformName({}) {}
	FsTextureHandle(const uint32_t& texture, const uint32_t& program, const uint32_t& count, const std::string name = {}) :
		TextureHandle (texture),
		ProgramHandle (program),
		SamplerCount  (count),
		UniformName   (name)
	{}
};

// texture array handle group. [texture3d]
struct FsTextureArrayHandle {
	uint32_t    TextureHandle, ProgramHandle;
	uint32_t    SamplerCount, TexArrayCount;
	std::string UniformName;

	FsTextureArrayHandle() : TextureHandle(0), ProgramHandle(0), SamplerCount(0), TexArrayCount(0), UniformName({}) {}
	FsTextureArrayHandle(const uint32_t& texture, const uint32_t& program, const uint32_t& tmu, const uint32_t& array, const std::string name = {}) :
		TextureHandle (texture),
		ProgramHandle (program),
		SamplerCount  (tmu),
		TexArrayCount (array),
		UniformName   (name)
	{}
};

struct FsMatrix3 { float matrix[3 * 3]; }; // matrix 3 * 3 fp32.
struct FsMatrix4 { float matrix[4 * 4]; }; // matrix 4 * 4 fp32.

struct FsVertex {
	float VertexCoord[FS_VERTEX_COORD];   // coordinate xyz.
	float VertexColor[FS_VERTEX_COLOR];   // color      rgba.
	float VertexTexture[FS_VERTEX_TEXUV]; // texture    uv.
	float VertexNormal[FS_VERTEX_COORD];  // normal     xyz.
};

// load model data_fmt.
struct FsRectangle { FsVertex Tvertex[FS_ELEMENT_VERTEX]; };
struct FSmodel {
	size_t       VertexDataLen;
	FsRectangle* QuadsModel;
};

// "FsVertexGroups" => vbo data_fmt.
struct FsVertexGroups {
	size_t       VertexDataLen;
	const float* VertexData;

	constexpr FsVertexGroups() : VertexDataLen(NULL), VertexData(nullptr) {}
	constexpr FsVertexGroups(const size_t& length, const float* pointer) :
		VertexDataLen (length),
		VertexData    (pointer)
	{}
};

#define FS_VERTEX_LENGTH  FS_VERTEX_COORD * 2 + FS_VERTEX_COLOR + FS_VERTEX_TEXUV
#define FS_VERTEX_BYTES  (FS_VERTEX_COORD * 2 + FS_VERTEX_COLOR + FS_VERTEX_TEXUV) * sizeof(float)

#define FS_SIZE_VERTEX  (size_t)52
#define FS_SIZE_NTEQUAD (size_t)156
#define FS_SIZE_MODEL   (size_t)16

struct FxRenderItem {

	FsModelHandle ModelHandle;

	uint32_t ShaderProgramHandle;
	uint32_t FrameBufferHandle;

	FsTextureHandle FrameBufferTex; // FBO 附着纹理. 

	// system param, timer & ortho mvp matrix.
	float     RenderTimer;
	FsMatrix4 RenderMatrixMvp;

	std::chrono::steady_clock::time_point RenderRateTimer;
	int64_t                               RenderRateTime;

	std::vector<FsTextureHandle>           ParamTexture;
	std::unordered_map<std::string, float> ParamValueFloat;

	bool ExecuteProgramFlag;

	FxRenderItem() :
		ModelHandle         (FsModelHandle()),
		ShaderProgramHandle (NULL),
		FrameBufferHandle   (NULL),
		FrameBufferTex      (FsTextureHandle()),
		RenderTimer         (0.0f),
		RenderMatrixMvp     ({}),
		ParamTexture        ({}),
		ParamValueFloat     ({}),
		ExecuteProgramFlag  (false),
		RenderRateTimer     (std::chrono::steady_clock::now()),
		RenderRateTime      (NULL)
	{}
};

#endif