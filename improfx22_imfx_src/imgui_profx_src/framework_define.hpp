// framework_define.

#ifndef _FRAMEWORK_DEFINE_HPP
#define _FRAMEWORK_DEFINE_HPP
#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include <random>

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

#define STAGE_EXIT_INIT -2
#define STAGE_EXIT_FREE -3
#define STAGE_EXIT_LOOP -4

#define RENDERLOOP_TIMER_STEP 0.025f

#define CVT_IMPTR    (void*)(intptr_t)
#define LAMBDA_QUOTE [&]()
#define LAMBDA_PARAM [=]()

enum LOGLABEL {
	LOG_ERROR   = 1 << 1, // 标签 <错误>
	LOG_WARNING = 1 << 2, // 标签 <警告>
	LOG_INFO    = 1 << 3, // 标签 <信息>
	LOG_TRACE   = 1 << 4, // 标签 <跟踪>
	LOG_PERF    = 1 << 5  // 标签 <性能>
};

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

struct FsMatrix3 { float matrix[3 * 3]; }; // matrix 3 * 3 fp32.
struct FsMatrix4 { float matrix[4 * 4]; }; // matrix 4 * 4 fp32.

// update[2023_11_27] 四边形图元 => 三角形图元.
#define FS_VERTEX_ELEMENT 3
#define FS_VERTEX_COORD   3
#define FS_VERTEX_COLOR   4
#define FS_VERTEX_TEXUV   2

#define FS_VERTEX_LENGTH  FS_VERTEX_COORD * 2 + FS_VERTEX_COLOR + FS_VERTEX_TEXUV
#define FS_VERTEX_BYTES  (FS_VERTEX_COORD * 2 + FS_VERTEX_COLOR + FS_VERTEX_TEXUV) * sizeof(float)

#define FS_SIZE_VERTEX   (size_t)52
#define FS_SIZE_TRIANGLE (size_t)156
#define FS_SIZE_MODEL    (size_t)16

struct FxRenderItem {

	std::string ShaderProgramItem;
	std::string FrameBufferItem;

	std::string ModelItem;
	// frame_buffer bind_render texture.
	std::string FrameBufferTexItem; 

	// system param, timer & ortho mvp matrix.
	float     RenderTimer;
	FsMatrix4 RenderMatrixMvp;

	std::chrono::steady_clock::time_point RenderRateTimer;
	int64_t                               RenderRateTime;

	std::vector<std::string>               ParamTextureItems;
	std::unordered_map<std::string, float> ParamValueFloat;

	bool ExecuteProgramFlag;

	FxRenderItem() :
		ModelItem          ({}),
		ShaderProgramItem  ({}),
		FrameBufferItem    ({}),
		FrameBufferTexItem ({}),
		RenderTimer        (0.0f),
		RenderMatrixMvp    ({}),
		ParamTextureItems  ({}),
		ParamValueFloat    ({}),
		ExecuteProgramFlag (false),
		RenderRateTimer    (std::chrono::steady_clock::now()),
		RenderRateTime     (NULL)
	{}
};

// framework define_module gen.
class IMPROFXDEF_GEN {
private:
	std::vector<int> UnqiueNumVector = {};
public:
	// generate unique number(random).
	int IMPROFXDEF_GenUniqueNumber() {
		int RandomValueTemp = NULL;
		bool LoopFlag = true;

		std::random_device Rand;
		std::mt19937 Gen(Rand());
		std::uniform_int_distribution<int> Dis(100000, 999999);

		do {
			RandomValueTemp = Dis(Gen);
			auto it = std::find(UnqiueNumVector.begin(), UnqiueNumVector.end(), RandomValueTemp);
			LoopFlag = it != UnqiueNumVector.end();
		} while (LoopFlag);

		UnqiueNumVector.push_back(RandomValueTemp);
		return RandomValueTemp;
	}
};

#endif