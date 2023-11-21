// framework_core. [2023_10_12] RCSZ.
// [2023_11_07]: OpenGL GLFW: 3.3.8, OpenGL 4.6 NVIDIA

#ifndef _FRAMEWORK_CORE_HPP
#define _FRAMEWORK_CORE_HPP

#include <variant>
#include <document.h> // RapidJSON

#include "../improfx_shader/framework_shader.h"
#include "../improfx_anim/framework_animation.hpp"
#include "../improfx_memory/framework_memory.hpp"
#include "../improfx_thread/framework_threadpool.hpp"

#include "framework_window.h"
#include "framework_imgui.h"

#define SYSMD_LB_RENDER std::string("[framework_core]: ")

namespace CoreModuleIMFX { class ChronoTimer; }

// move(async(func, args)).
template <typename Func, typename... Args>
auto imfx_move_async(Func&& func, Args&&... args) {
	return std::async(std::launch::async, std::forward<Func>(func), std::forward<Args>(args)...);
}

// warning: not thread-safe.
namespace ImProFxTools {
	typedef const char* strkey;

	struct SrcImage {
		uint8_t* imgpixel_ptr;
		// image: width(x), height(y), channels(color).
		uint32_t img_width, img_height, img_channels;
	};

	class FrameworkDataFind {
	public:
		FxRenderItem FindRenderItem(std::unordered_map<std::string, FxRenderItem>& input, strkey itemkey);
		uint32_t     FindRenderItemFBO(std::unordered_map<std::string, FxRenderItem>& input, strkey itemkey);

		void FindSetRenderItemParam(std::unordered_map<std::string, FxRenderItem>& input, strkey itemkey, strkey uniform_name, float value);
		void FindSetRenderItemFlag(std::unordered_map<std::string, FxRenderItem>& input, strkey itemkey, bool flag);

		uint32_t FindRawTextureImg(std::unordered_map<std::string, FStextureHandle>& input, strkey itemkey);

		void FindLoadRawTextureImgData(std::unordered_map<std::string, FStextureHandle>& input, strkey itemkey, const SrcImage& data);
		void FindLoadRawTextureImgFile(std::unordered_map<std::string, FStextureHandle>& input, strkey itemkey, const std::string& file);
	};

	using CoreModuleIMFX::ChronoTimer;
	class FrameworkStaticFind {
	public:
		void  StaticFlagAlloc(std::unordered_map<std::string, bool>& input, strkey itemkey, bool init = false, bool REPEAT = false);
		void  StaticFlagDelete(std::unordered_map<std::string, bool>& input, strkey itemkey);
		bool* StaticFlagFind(std::unordered_map<std::string, bool>& input, strkey itemkey);

		void         StaticTimerAlloc(std::unordered_map<std::string, ChronoTimer*>& input, strkey itemkey);
		void         StaticTimerDelete(std::unordered_map<std::string, ChronoTimer*>& input, strkey itemkey);
		ChronoTimer* StaticTimerFind(std::unordered_map<std::string, ChronoTimer*>& input, strkey itemkey);
	};

	class FrameworkTaskFind {
	public:
		void TaskAsyncCreate(std::unordered_map<std::string, std::shared_future<void>>& input, strkey itemkey, std::future<void> async);
		// retained: false: valid = true? => erase task.
		bool TaskAsyncPromise(
			std::unordered_map<std::string, std::shared_future<void>>& input,
			strkey                                                     itemkey, 
			const bool&                                                reserve
		);
	};

	// consider async load config. 
	class FrameworkInline {
	protected:
		const float matrix_ortho_cube = 10.0f;

		std::string VertexShaderScript   = {};
		std::string FragmentShaderScript = {};
		
		void ShaderFragmentInline(const std::string text);

	public:
		FrameworkInline(const std::string text) {
			ShaderFragmentInline(text);
		}
		void ShaderProgramConfig(
			std::unordered_map<std::string, FxRenderItem>& input, 
			const char*                                    key, 
			const Vector2T<float>                          resolution, 
			const int64_t                                  rate
		);
	};

	extern FrameworkDataFind   FrmData;
	extern FrameworkStaticFind FrmStatic;
	extern FrameworkTaskFind   FrmTask;
}

namespace CoreModuleIMFX {
	class INTERFACE_PROFX_USER;
	
	class ChronoTimer {
	protected:
		std::chrono::steady_clock::time_point PeriodicTimer = std::chrono::steady_clock::now(); // 周期性触发计时.
		std::chrono::steady_clock::time_point ContextTimer  = std::chrono::steady_clock::now(); // 上下文时间测定.
		int64_t TimeCount = NULL;

	public:
		// unit: milliseconds.
		bool PeriodicTimerFlag(float itl_time, float* loss_time = nullptr);
		// context timer: begin_timer func{ code } end_timer, unit: milliseconds.
		float ContextTimerUse(std::function<void()> context_function);
	};

	struct FRMCORE_PACKAGE {
		std::unordered_map<std::string, FxRenderItem>                   DataShaderFx;   // shader fx render.
		std::unordered_map<std::string, FStextureHandle>                DataTextureRaw; // raw texture.
		std::unordered_map<std::string, ImGuiProAnim::ImAnimationBase*> DataAnimation;  // animation component.
		std::unordered_map<std::string, ImDynamicMem::ImMemory*>        DataDynamicMem; // dynamic memory.

		std::unordered_map<std::string, bool>         StaticFlag;  // static boolean flags.
		std::unordered_map<std::string, ChronoTimer*> StaticTimer; // static timer.

		std::unordered_map<std::string, std::shared_future<void>> TaskAsync;      // execute async task.
		ImThreadTask::ThreadPool                                  TaskThreadPool; // thread pool.

		FRMCORE_PACKAGE() :
			DataShaderFx   ({}),
			DataTextureRaw ({}),
			DataAnimation  ({}),
			DataDynamicMem ({}),
			StaticFlag     ({}),
			StaticTimer    ({}),
			TaskAsync      ({}),
			TaskThreadPool (DEF_INIT_THREAD_WORKERS)
		{}
	};

	struct regobjectinfo {
		std::string ClassName;
		std::string UniqueObjName;

		regobjectinfo(std::string cn, std::string uon) :
			ClassName     (cn),
			UniqueObjName (uon)
		{}
	};

	struct FRMCORE_INFO {
		Vector2T<float> WindowSize;       // x:width, y:height.
		float           FrameSmoothValue; // render_loop tick scale.

		Vector2T<float> MousePosition; // [GLFWAPI]
		bool            WindowFoucs;   // [GLFWAPI]

		// 文件(s)被拖拽到窗口(文件路径) [GLFWAPI]
		std::vector<std::string>* DropFiles;
		void DropFilesOK() {

			DropFiles->clear();
			DropFiles->shrink_to_fit();
		}

		// frame unique number(imgui PushID)(next frame clear id_count).
		uint32_t _UniqueCountID;
		uint32_t GetUniqueID() {
			return ++_UniqueCountID;
		}

		// generate random func.
		std::function<float(float, float)> _RandomFunction;
		float GetFloatRandom(float min, float max) {
			return _RandomFunction(min, max);
		}

		// register object number(std::vector::size).
		// register object unique_name, class_name.
		std::vector<regobjectinfo> USER_REGISTER_INFO_NAME;

		FRMCORE_INFO() :
			WindowSize              ({}),
			FrameSmoothValue        (1.0f),
			MousePosition           ({}),
			WindowFoucs             (false),
			DropFiles               (nullptr),
			_UniqueCountID          (NULL),
			_RandomFunction         ({}),
			USER_REGISTER_INFO_NAME ({})
		{}
	};

	// frameowrk gui_logic interface(reg).
	class INTERFACE_PROFX_USER {
	public:
		// init: return flag: false:failed, true:success.
		virtual bool LogicInitialization(FRMCORE_PACKAGE& FrmDat) = 0;
		// loop: return flag: false:continue, true:exit.
		virtual bool LogicEventLoop(FRMCORE_PACKAGE& FrmDat, FRMCORE_INFO& FrmInfo) = 0;
		virtual void LogicCloseFree() = 0;
	};

	class FrameworkImCore :public INTERFACE_PROFX_START,
		CoreModuleWindow::RenderWindow,
		CoreModuleImGui::RenderImGui
	{
	private:
		// random numbers. [HighCost]
		static float GenerateRandomNumbers(float value_min, float value_max);

	protected:
		float TimerSmoothCount = 100.0f; // start run_speed 100.0f(fps)
		std::chrono::steady_clock::time_point SampleTimer = std::chrono::steady_clock::now();

		rapidjson::Document ReadJsonConfig(std::string file);
		size_t ShaderConfigLoad(rapidjson::Document config);

		// render tick function.
		void RenderSceneFrameBuffer();
		// fps smooth calc.
		void CalculateSmoothTrans(float preset_fps, int64_t sample_time);
		// free FREMCORE_HASHMAP data.
		void ResourcesUnorderedMapFree();

		// user registration gui_logic class data.
		std::unordered_map<std::string, INTERFACE_PROFX_USER*> COREDATAUSEROBJ = {};
		// core data(hash_map) pack & system info.
		FRMCORE_PACKAGE COREDATAPACK = {};
		FRMCORE_INFO    COREDATAINFO = {};

		// max number texture mapping unit.
		uint32_t MaxNumberTMU = NULL;

		// time tick scale, value sample.
		float TimerSmoothValue = 1.0f;

	public:
		void UserRegistrationObject(const std::string& RegClassName, const std::string& RegUniqueName, INTERFACE_PROFX_USER* RegClass);
		void UserDeleteObject(const std::string& UniqueName);
		// find uesr registration(reflection).
		INTERFACE_PROFX_USER* UserFindRegObject(const std::string& RegUniqueName);

		bool    ProfxInit();      // frm_interface system initialize.
		bool    ProfxFree();      // frm_interface system free(delete).
		int32_t ProfxEventLoop(); // frm_interface system run_loop.
	};
}

void RegUserClassFunction();

namespace CoreModuleSystem {
	extern CoreModuleIMFX::FrameworkImCore* framework_global_object;
	
	class ReflectionFactory {
	public:
		using CreateFunction = std::function<CoreModuleIMFX::INTERFACE_PROFX_USER* ()>;

		static std::unordered_map<std::string, CreateFunction>& GetRegistry() {
			static std::unordered_map<std::string, CreateFunction> registry;
			return registry;
		}
	};

	template<typename T>
	class Registrar {
	public:
		Registrar(const std::string& className) {
			auto CreateFunction = []() -> CoreModuleIMFX::INTERFACE_PROFX_USER* {
				return new T();
			};
			// register.
			auto& registry = ReflectionFactory::GetRegistry();
			registry[className] = CreateFunction;
		}
	};

	// => "framework_core_system.cpp"
	class ProfxSystemFramework {
	public:
		void FrameworkRegStart(CoreModuleIMFX::FrameworkImCore* OBJ) {
			framework_global_object = OBJ;
		}
		int32_t StartProfx();
	};

	class ProfxReflection {
	public:
		// framework user_object 获取对象.
		template <typename UserObj>
		UserObj* ReflectionObjectGet(const std::string& ObjectName) {
			// "FrameworkImCore" => "ProfxSystemFramewrk"
			// get registration object.
			CoreModuleIMFX::INTERFACE_PROFX_USER* objptr = framework_global_object->UserFindRegObject(ObjectName);
			if (objptr) {
				if (typeid(*objptr) == typeid(UserObj))
					return dynamic_cast<UserObj*>(objptr);
				else
					LOGCONS::LogInfoPush(LOG_ERR, "[reflection]: failed: object is not of the expected type.");
			}
			else
				LOGCONS::LogInfoPush(LOG_ERR, "[reflection]: failed: object = nullptr.");
			return nullptr;
		}

		// framework user_object 创建对象.
		void ReflectionObjectCreate(const std::string& ClassName, const std::string& ObjectName);
		// framework user_object 销毁对象.
		void ReflectionObjectDelete(const std::string& UniqueObjectName);
	};
}
// framework management global objects.
extern CoreModuleSystem::ProfxSystemFramework IMFX_SYSTEM_OBJ;
// framework reflection call global objects.
extern CoreModuleSystem::ProfxReflection IMFXREF;

// 注册用户类 (inheritance "INTERFACE_PROFX_USER").
#define FXUCLASS(UserClass) \
    inline static CoreModuleSystem::Registrar<UserClass> UserClass##Registrar(#UserClass)

#endif