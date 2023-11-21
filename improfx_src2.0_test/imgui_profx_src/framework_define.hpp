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

typedef unsigned int FSGLuint;

template <typename mvec>
// vector 2d x, y.
struct Vector2T {
	mvec vector_x, vector_y;
	constexpr Vector2T() : vector_x(0), vector_y(0) {}
	constexpr Vector2T(mvec x, mvec y) : vector_x(x), vector_y(y) {}
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

#define FS_ELEMENT_VERTEX 4
#define FS_VERTEX_COORD   3
#define FS_VERTEX_COLOR   4
#define FS_VERTEX_TEXUV   2

// vertex buffer object, vertex array object.
struct FSmodelHanlde {

	FSGLuint vao_handle, vbo_handle;

	constexpr FSmodelHanlde() : vao_handle(0), vbo_handle(0) {}
	constexpr FSmodelHanlde(uint32_t vao, uint32_t vbo) :
		vao_handle (vao), 
		vbo_handle (vbo)
	{}
};

// texture handle, program handle.
struct FStextureHandle {

	FSGLuint    texture_hd, program_hd;
	uint32_t    tmu_count;
	std::string uniform_name;

	FStextureHandle() : texture_hd(0), program_hd(0), tmu_count(0), uniform_name({}) {}
	FStextureHandle(uint32_t tex, uint32_t pgm, uint32_t count, std::string name = "") :
		texture_hd   (tex),
		program_hd   (pgm),
		tmu_count    (count),
		uniform_name (name)
	{}
};

struct FSmatrix3 { float matrix[3 * 3]; }; // matrix 3 * 3 fp32.
struct FSmatrix4 { float matrix[4 * 4]; }; // matrix 4 * 4 fp32.

struct FSvertex {
	float verCoord[FS_VERTEX_COORD];   // coordinate xyz.
	float verColor[FS_VERTEX_COLOR];   // color      rgba.
	float verTexture[FS_VERTEX_TEXUV]; // texture    uv.
	float verNormal[FS_VERTEX_COORD];  // normal     xyz.
};

// load model data_fmt.
struct FSrectangle { FSvertex Tvertex[FS_ELEMENT_VERTEX]; };
struct FSmodel {
	size_t       ModelDataLength;
	FSrectangle* QuadsModel;
};

// "FSdynamic" => vbo data_fmt.
struct FSdynamic {
	size_t ModelDataLength;
	float* VertexData;
};

#define FS_VERTEX_LENGTH  FS_VERTEX_COORD * 2 + FS_VERTEX_COLOR + FS_VERTEX_TEXUV
#define FS_VERTEX_BYTES  (FS_VERTEX_COORD * 2 + FS_VERTEX_COLOR + FS_VERTEX_TEXUV) * sizeof(float)

#define FS_SIZE_VERTEX  (size_t)52
#define FS_SIZE_NTEQUAD (size_t)156
#define FS_SIZE_MODEL   (size_t)16

// framework start interface.
class INTERFACE_PROFX_START {
protected:
	virtual bool    ProfxInit()      = 0;
	virtual bool    ProfxFree()      = 0;
	virtual int32_t ProfxEventLoop() = 0;
};

struct FxRenderItem {

	FSmodelHanlde ModelHandle;

	FSGLuint ShaderProgramHandle;
	FSGLuint FrameBufferHandle;

	FStextureHandle FrameBufferTex; // FBO ¸½×ÅÎÆÀí. 

	// system param, timer & ortho mvp matrix.
	float     RenderTimer;
	FSmatrix4 RenderMVPmatrix;

	std::chrono::steady_clock::time_point RenderRateTimer;
	int64_t                               RenderRateTime;

	std::vector<FStextureHandle>           ParamTexture;
	std::unordered_map<std::string, float> ParamValueFloat;

	bool ExecuteProgramFlag;

	FxRenderItem() :
		ModelHandle         (FSmodelHanlde()),
		ShaderProgramHandle (NULL),
		FrameBufferHandle   (NULL),
		FrameBufferTex      (FStextureHandle()),
		RenderTimer         (0.0f),
		RenderMVPmatrix     ({}),
		ParamTexture        ({}),
		ParamValueFloat     ({}),
		ExecuteProgramFlag  (false),
		RenderRateTimer     (std::chrono::steady_clock::now()),
		RenderRateTime      (NULL)
	{}
};

#endif