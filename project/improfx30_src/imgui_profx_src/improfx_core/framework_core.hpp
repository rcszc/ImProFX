// framework_core. [2023_10_12] RCSZ.
// [2023_11_07]: OpenGL GLFW: 3.3.8, OpenGL 4.6 NVIDIA

#ifndef _FRAMEWORK_CORE_HPP
#define _FRAMEWORK_CORE_HPP
// Json Loader: "RapidJSON"
#include <document.h>

#include "../improfx_renderer/framework_imfxgl_resource.hpp"
#include "../improfx_animation/framework_animation.h"
#include "../improfx_memory/framework_memory.hpp"
#include "../improfx_thread/framework_tpool.hpp"
#include "../improfx_particle/framework_particle.h"

#include "framework_core_interface.hpp"
#include "framework_window.h"
#include "framework_imgui.h"

#define SYSMD_LB_CORE "[FRAMEWORK_CORE]: "

namespace CoreModuleIMFX { class ChronoTimer; }

template<typename TargetType>
TargetType ImfxConvertAny(const std::any& source) {
	if (source.type() == typeid(TargetType))
		return std::any_cast<TargetType>(source);
	else
		return TargetType();
}

ImVec4 ImColorDepthScale(const ImVec4& color, float value, float alpha);

struct TraceOperations {
	size_t TraceOperationAllocating;
	size_t TraceOperationDelete;

	constexpr TraceOperations() : TraceOperationAllocating(NULL), TraceOperationDelete(NULL) {};
	bool ComparingOper() {
		return TraceOperationAllocating == TraceOperationDelete ? true : false;
	}
};

// warning: not thread-safe.
namespace ImProFxTools {
#define	STCMODE_DELETE         0xAC01 // [key] delete object & erase item.
#define	STCMODE_DELETE_NOERASE 0xAC02 // [key] delete object
#define	STCMODE_DELETE_ALL     0xAC03 // [all] delete object & erase item.
	typedef const int&  deletemode;
	typedef const char* strkey;
	
	struct SrcImage {
		// image: width(x), height(y), channels(color).
		uint32_t ImageWidth, ImageHeight, ImageChannels;
		uint8_t* ImagePixelsPointer;
	};

	class FrameworkCoreManager :protected IMPROFX_LOW::IMPROFX_DATASET_LLRES {
	public:
		FxRenderGroup  FindGetRenderItem(std::unordered_map<std::string, FxRenderGroup>& input, strkey itemkey);
		FxRenderGroup* FindGetRenderMapping(std::unordered_map<std::string, FxRenderGroup>& input, strkey itemkey);
		uint32_t       FindGetRenderTexture(std::unordered_map<std::string, FxRenderGroup>& input, strkey itemkey);

		void FindSetRenderItemParam(std::unordered_map<std::string, FxRenderGroup>& input, strkey itemkey, strkey UniformName, float value);
		void FindSetRenderItemFlag(std::unordered_map<std::string, FxRenderGroup>& input, strkey itemkey, bool flag);

		ImfxTexture FindGetRawImage(std::unordered_map<std::string, std::string>& input, strkey itemkey);
	};

	class FrameworkCoreOper :protected IMPROFX_LOW::IMPROFX_DATASET_LLRES {
	public:
		bool FindLoadRenderItem(std::unordered_map<std::string, FxRenderGroup>& input, strkey itemkey, FxRenderGroup fxshader);
		bool FindDeleteRenderItem(std::unordered_map<std::string, FxRenderGroup>& input, strkey itemkey);

		bool FindLoadRawImageData(std::unordered_map<std::string, std::string>& input, strkey itemkey, const SrcImage& data);
		bool FindLoadRawImageFile(std::unordered_map<std::string, std::string>& input, strkey itemkey, const std::string& file);
		bool FindDeleteRawImage(std::unordered_map<std::string, std::string>& input, strkey itemkey);
	};

	using CoreModuleIMFX::ChronoTimer;
	class FrameworkModule {
	protected:
		TraceOperations ModuleOperTrace = {};
	public:
		bool  FlagModuleCreate(std::unordered_map<std::string, bool>& input, strkey itemkey, bool init = false, bool duplicate = false);
		bool  FlagModuleDelete(std::unordered_map<std::string, bool>& input, strkey itemkey, deletemode mode = STCMODE_DELETE);
		bool* FlagModuleFind(std::unordered_map<std::string, bool>& input, strkey itemkey);

		bool         TimerModuleCreate(std::unordered_map<std::string, ChronoTimer*>& input, strkey itemkey);
		bool         TimerModuleDelete(std::unordered_map<std::string, ChronoTimer*>& input, strkey itemkey, deletemode mode = STCMODE_DELETE);
		ChronoTimer* TimerModuleFind(std::unordered_map<std::string, ChronoTimer*>& input, strkey itemkey);

		bool ParticleModuleCreate(std::unordered_map<std::string, IFC_PARTC::ParticleBase*>& input, strkey itemkey, const Vector2T<float>& windowsize);
		bool ParticleModuleDelete(std::unordered_map<std::string, IFC_PARTC::ParticleBase*>& input, strkey itemkey, deletemode mode = STCMODE_DELETE);
		IFC_PARTC::ParticleBase* ParticleModuleFind(std::unordered_map<std::string, IFC_PARTC::ParticleBase*>& input, strkey itemkey);

		// vector_x: create, vector_y: delete.
		bool GetOperInfo(Vector2T<size_t>& number);
	};

	class FrameworkVariable {
	public:
		bool VariableAnyCreate(std::unordered_map<std::string, std::any>& input, strkey itemkey, std::any init = {});
		bool VariableAnyDelete(std::unordered_map<std::string, std::any>& input, strkey itemkey);

		bool VariableAnyModify(std::unordered_map<std::string, std::any>& input, strkey itemkey, std::any value);
		std::any VariableAnyFindGet(std::unordered_map<std::string, std::any>& input, strkey itemkey);
	};

	extern FrameworkCoreManager FrmCore;
	extern FrameworkCoreOper    FrmCoreOper;
	extern FrameworkModule      FrmModule;
	extern FrameworkVariable    FrmVariable;
}

namespace CoreModuleIMFX {
	class INTERFACE_PROFX_USER;
	
	class ChronoTimer {
	protected:
		std::chrono::steady_clock::time_point PeriodicTimer = std::chrono::steady_clock::now(); // 周期性触发计时.
		std::chrono::steady_clock::time_point ContextTimer  = std::chrono::steady_clock::now(); // 上下文时间测定.
		
	public:
		// unit: milliseconds.
		bool PeriodicTimerFlag(float itl_time, float* loss_time = nullptr);
		// context timer: begin_timer func{ code } end_timer, unit: milliseconds.
		float ContextTimerUse(std::function<void()> context_function);
	};

	struct FRMCORE_PACKAGE {
		// system dynamic background. (load bg => exe_flag)
		FxRenderGroup BackgroundShaderFX;

		std::unordered_map<std::string, FxRenderGroup>                 DataShaderFx;       // shader fx render. [TWOLEVEL_MAPPING]
		std::unordered_map<std::string, std::string>                   DataTextureRaw;     // raw texture.      [TWOLEVEL_MAPPING]

		std::unordered_map<std::string, IFC_ANIM::ImAnimBaseStateful*> DataAnimation;      // animation component.
		std::unordered_map<std::string, IFC_ANIM::ImAnimBaseDynamic*>  DataAnimationNodes; // animation nodes component.
		std::unordered_map<std::string, IFC_MEMORY::ImMemoryBase*>     DataDynamicMem;     // dynamic memory.

		std::unordered_map<std::string, bool>         StaticFlag;  // [stc] boolean flags.
		std::unordered_map<std::string, ChronoTimer*> StaticTimer; // [stc] timer.

		std::unordered_map<std::string, std::any> VariablePool;   // data variable pool.
		ImThreadTask::WorkPool*                   TaskThreadPool; // thread pool.

		std::unordered_map<std::string, IFC_PARTC::ParticleBase*> ParticleSystem; // [stc] particle component.
		
		// framework_core .json file loader('RapidJSON').
		std::function<rapidjson::Document(const std::string&, bool&)> JsonFileLoader;

		FRMCORE_PACKAGE() :
			BackgroundShaderFX ({}),
			DataShaderFx       ({}),
			DataTextureRaw     ({}),
			DataAnimation      ({}),
			DataDynamicMem     ({}),
			StaticFlag         ({}),
			StaticTimer        ({}),
			VariablePool       ({}),
			TaskThreadPool     (nullptr),
			ParticleSystem     ({})
		{}
	};

	struct RegisterObjectInfo {
		std::string ClassName;
		std::string UniqueObjectName;

		RegisterObjectInfo(std::string class_name, std::string obj_name) :
			ClassName        (class_name),
			UniqueObjectName (obj_name)
		{}
	};

	// store in framework.
	struct RenderGuiFinalProcessing {
		// render_fx => imgui => gui_final => out_screen.
		std::string ShaderProgramItem;
		std::string FrameBufferItem;

		std::string ModelItem;
		// frame_buffer bind_render texture.
		std::string FrameBufferProcessTex;

		FsMatrix4 RenderMatrixMvp;
	};

	// store in framecore_info => framework.
	struct RenderGuiFinalSettings {
		// gui_global fx_settings.
		bool            FxEnableBloom;     // 开启全局辉光.
		int32_t         FxBloomRadius;     // 辉光半径.
		float           FxBloomFilter;     // 辉光颜色过滤(RGB.AVG > FILT).
		Vector2T<float> FxBloomColorBlend; // 辉光颜色混合(SRC & BLUR).

		RenderGuiFinalSettings() : FxEnableBloom(false), FxBloomRadius(5), FxBloomFilter(0.1f), FxBloomColorBlend(Vector2T<float>(0.58f, 1.0f)) {}
	};

	struct FRMCORE_INFO {
		Vector2T<float> MousePosition; // system mouse_pos. [GLFWAPI]
		Vector2T<float> WindowSize;    // x:width, y:height.

		float FrameSmoothValue; // render_loop tick scale.
		bool  WindowFoucs;      // system window_foucs. [GLFWAPI]

		// ImProFX GUI(ImGui) Object, ImGui_FUNC => OBJ => INFO.
		ImProFXgui::FrameworkGUI* ImProFXGUI;

		// 文件(s)被拖拽到窗口时(文件路径) [GLFWAPI]
		std::vector<std::string>* DropFiles;
		void DropFilesOK() {
			// free filepath array.
			DropFiles->clear();
			DropFiles->shrink_to_fit();
		}

		RenderGuiFinalSettings GuiFinalFxSettings;

		// register object number(std::vector::size).
		// register object unique_name, class_name.
		std::vector<RegisterObjectInfo> UserRegistrationInfo;

		// framework main color_system.
		ImVec4 MainColorSystem;

		// frame unique number(imgui PushID)(next frame clear id_count).
		std::atomic<uint32_t>* S_UniqueCountID;
		uint32_t GetUniqueID() {
			return ++*S_UniqueCountID;
		}
		void GetUniqueIDArray(uint32_t* idptr, size_t length) {
			for (size_t i = 0; i < length; ++i)
				idptr[length] = ++*S_UniqueCountID;
		}

		// generate random func.
		std::function<float(float, float)> S_RandomFunction;
		float GetFloatRandom(float min, float max) {
			return S_RandomFunction(min, max);
		}

		uint32_t S_DeveloperTestHandle;  // 开发测试用.句柄[框架生命周期内]
		void*    S_DeveloperTestPointer; // 开发测试用.指针[框架生命周期内]

		FRMCORE_INFO() :
			WindowSize             ({}),
			FrameSmoothValue       (1.0f),
			MousePosition          ({}),
			WindowFoucs            (false),
			ImProFXGUI             (nullptr),
			DropFiles              (nullptr),
			GuiFinalFxSettings     ({}),
			UserRegistrationInfo   ({}),
			MainColorSystem        (ImVec4(0.0f,1.0f,1.0f,0.92f)),
			S_UniqueCountID        (nullptr),
			S_RandomFunction       ({}),
			S_DeveloperTestHandle  (NULL),
			S_DeveloperTestPointer (nullptr)
		{}
	};

	// frameowrk gui_logic interface.
	class INTERFACE_PROFX_USER {
	public:
		// init: return flag: false:failed, true:success.
		virtual bool LogicInitialization(FRMCORE_PACKAGE& FrmDat) = 0;
		// loop: return flag: false:continue, true:exit.
		virtual bool LogicEventLoop(FRMCORE_PACKAGE& FrmDat, FRMCORE_INFO& FrmInfo) = 0;
		virtual void LogicCloseFree() = 0;
	};

	class FrameworkImCore :public INTERFACE_PROFX_START, protected IMPROFX_LOW::IMPROFX_DATASET_LLRES,
		public CoreModuleWindow::RenderWindow, 
		public CoreModuleImGui::RenderImGui
	{
	private:
		// random numbers. [HighCost]
		static float GenerateRandomNumbers(float value_min, float value_max);
		// read .json configuration file.
		static rapidjson::Document JsonConfigLoader(const std::string& file, bool& state);

		// shader oper(low_layer_management).
		IMFX_OGL_MAG::ImFXOGLinit    LLMAG_RendererInit = {};
		IMFX_OGL_MAG::ImFxOGLuniform LLMAG_Uniform      = {};

	protected:
		// config(alloc) shader_res.
		size_t GraphicsLoadingConfig(rapidjson::Document config);

		// render tick function.
		void UpdateFXshaderFrameBuffer();
		void UpdateCoreModuleObject();
		// fps smooth calc.
		void CalculateSmoothTrans(float preset_fps, int64_t sample_time);
		// free core_resources data.
		void ResourcesPackageFree(FRMCORE_PACKAGE& package);
		void ResourcesInfoFree(FRMCORE_INFO& dyinfo);

		// user registration gui_logic class data.
		std::unordered_map<std::string, INTERFACE_PROFX_USER*> COREDATAUSEROBJ = {};

		// core data(hash_map) pack & system info.
		FRMCORE_PACKAGE COREDATAPACK = {};
		FRMCORE_INFO    COREDATAINFO = {};

		// gui final processing stage.
		RenderGuiFinalProcessing GuiFxProcess = {};

		// system fullwindow background 'method'.
		std::function<void(ImfxShader BgShHD)> BackgroundFunction = [](ImfxShader) {};

		// max number texture mapping unit.
		uint32_t DeviceTmuMax = NULL;

		std::chrono::steady_clock::time_point SampleTimer = std::chrono::steady_clock::now();
		// time tick scale, value sample.
		float TimerSmoothValue = 1.0f;
		float TimerSmoothCount = 100.0f; // start run_speed 100.0f(fps)

	public:
		void UserRegistrationObject(const char* class_name, const char* unique_name, INTERFACE_PROFX_USER* reg_class);
		void UserDeleteObject(const char* unique_name);
		// find uesr registration(reflection).
		INTERFACE_PROFX_USER* UserFindRegObject(const char* unique_name);

		bool    ProFxCoreInit(const char* config_file) override; // frm_interface system initialize.
		bool    ProFxCoreFree() override;                        // frm_interface system free(delete).
		int32_t ProFxCoreEventloop() override;                   // frm_interface system run_loop.
	};
}

namespace CoreModuleSystem {
	extern CoreModuleIMFX::FrameworkImCore* FrameworkCoreGlobalObject;
	
	class ReflectionFactory {
	public:
		using CreateFunction = std::function<CoreModuleIMFX::INTERFACE_PROFX_USER*()>;

		static std::unordered_map<std::string, CreateFunction>& GetRegistry() {
			static std::unordered_map<std::string, CreateFunction> Registry;
			return Registry;
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
			auto& Registry = ReflectionFactory::GetRegistry();
			Registry[className] = CreateFunction;
		}
	};

	// => "framework_core_system.cpp"
	class ProfxSystemFramework {
	protected:
		void InitialStageRegUserClass();
	public:
		void FrameworkRegStart(CoreModuleIMFX::FrameworkImCore* OBJ) {
			FrameworkCoreGlobalObject = OBJ;
		}
		int32_t StartProfx();
		const char* FrameworkConfigFilePath = FrameworkSystemPath[1];
	};

	class ProfxReflection :public INTERFACE_REF_USERCLASS {
	public:
		template <typename UserObj>
		// framework user_object 获取对象.
		UserObj* ReflectionObjectFind(const char* UniqueName) {
			// "FrameworkImCore" => "ProfxSystemFramewrk"
			// get registration object.
			CoreModuleIMFX::INTERFACE_PROFX_USER* objptr = FrameworkCoreGlobalObject->UserFindRegObject(UniqueName);
			if (objptr) {
				if (typeid(*objptr) == typeid(UserObj))
					return dynamic_cast<UserObj*>(objptr);
				else
					LOGCONS::PushLogger(LOG_ERROR, "[reflection]: ", "failed: object is not of the expected type.");
			}
			else
				LOGCONS::PushLogger(LOG_ERROR, "[reflection]: ", "failed: object = nullptr.");
			return nullptr;
		}

		// framework user_object 创建对象.
		void ReflectionObjectCreate(const char* ClassName, const char* UniqueName);
		// framework user_object 销毁对象.
		void ReflectionObjectDelete(const char* UniqueName);
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