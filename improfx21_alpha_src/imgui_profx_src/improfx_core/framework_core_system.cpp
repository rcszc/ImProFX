// framework_core_system.
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "framework_core.hpp"

using namespace std;
using namespace LOGCONS;

namespace CoreModuleSystem {
	CoreModuleIMFX::FrameworkImCore* FrameworkCoreGlobalObject = nullptr;

	int32_t ProfxSystemFramework::StartProfx() {
		int32_t ReturnLoopFlag = 1;
		int32_t ReturnMainFlag = NULL;

		// start logger process (默认框架系统文件夹).
		LOGFILE::StartLogFileProcess("imgui_profx_system/syslog/");
		// register_user class,object.
		InitialStageRegUserClass();

		if (FrameworkCoreGlobalObject->ProFxCoreInit()) {

			while (ReturnLoopFlag == 1) {
				ReturnLoopFlag = FrameworkCoreGlobalObject->ProFxCoreEventloop();
			}
			if (ReturnLoopFlag == -1)
				ReturnMainFlag = STAGE_EXIT_LOOP;

			if (!FrameworkCoreGlobalObject->ProFxCoreFree())
				ReturnMainFlag = STAGE_EXIT_FREE;

			delete FrameworkCoreGlobalObject;
		}
		else
			ReturnMainFlag = STAGE_EXIT_INIT;

		LOGFILE::FreeLogFileProcess(); // free process thread.
		return ReturnMainFlag;
	}

	void ProfxReflection::ReflectionObjectCreate(const char* ClassName, const char* UniqueName) {
		auto& Registry = ReflectionFactory::GetRegistry();

		auto it = Registry.find(ClassName);
		if (it != Registry.end())
			FrameworkCoreGlobalObject->UserRegistrationObject(ClassName, UniqueName, it->second());
		else
			LOGCONS::PushLogger(LOG_ERR, "[reflection]: ", "failed: class not found in the registry.");
	}

	void ProfxReflection::ReflectionObjectDelete(const char* UniqueName) {
		FrameworkCoreGlobalObject->UserDeleteObject(UniqueName);
	}
}

CoreModuleSystem::ProfxSystemFramework IMFX_SYSTEM_OBJ;
CoreModuleSystem::ProfxReflection      IMFXREF;

namespace CoreModuleIMFX {

	rapidjson::Document FrameworkImCore::ReadJsonConfig(const string& file, bool& state) {
		rapidjson::Document JsonDoc;

		ifstream ReadJsonFile(file);
		if (ReadJsonFile.is_open()) {
			// read char file.
			string JsonDocString((istreambuf_iterator<char>(ReadJsonFile)), istreambuf_iterator<char>());
			ReadJsonFile.close();

			JsonDoc.Parse(JsonDocString.c_str());
			PushLogger(LOG_INFO, SYSMD_LB_CORE, "read json config file: %s", file.c_str());
			state = true;
		}
		else {
			PushLogger(LOG_WARN, SYSMD_LB_CORE, "failed read json config.");
			state = false;
		}
		return JsonDoc;
	}
	
	// OpenGL图形加载&配置.
	size_t FrameworkImCore::GraphicsLoadingConfig(rapidjson::Document config) {
		size_t return_items = NULL;
		// VAO create flag(count).
		uint32_t VerAttCreateFlag = NULL;
		uint32_t VerBuffer = NULL;

		// set vertex attribute count_begin.
		SFglobalTransmitData.set_location_begin(0);

		if (!config.HasParseError() && config.IsObject()) {
			if (config.HasMember("core_shaderfx_config") && config["core_shaderfx_config"].IsArray()) {
				for (const auto& FxConfig : config["core_shaderfx_config"].GetArray()) {

					const char* KeyNameTemp = {};

					if (FxConfig.HasMember("shaderfx_package") && FxConfig["shaderfx_package"].IsString())
						KeyNameTemp = FxConfig["shaderfx_package"].GetString();
					
					// ################################################ ShaderLoading ################################################
					// "ImProFxTools::FxShaderLoader". [2023_12_10]

					string ShaderScriptTemp[2] = {};

					if (FxConfig.HasMember("shader_file_vertex") && FxConfig["shader_file_vertex"].IsString())
						ShaderScriptTemp[0] = FxConfig["shader_file_vertex"].GetString();

					if (FxConfig.HasMember("shader_file_fragment") && FxConfig["shader_file_fragment"].IsString())
						ShaderScriptTemp[1] = FxConfig["shader_file_fragment"].GetString();

					ImProFxTools::FxShaderLoader ShaderLoader(ShaderScriptTemp[0], ShaderScriptTemp[1]);

					uint32_t ShaderProgramTemp = ShaderLoader.FxShaderProgramCreateCompile();

					float FrameRateTime = 0.0f;
					if (FxConfig.HasMember("render_max_fps") && FxConfig["render_max_fps"].IsFloat())
						FrameRateTime = FxConfig["render_max_fps"].GetFloat();

					Vector2T<float> RenderScale = {};
					if (FxConfig.HasMember("render_scale_xy") && FxConfig["render_scale_xy"].IsArray()) {
						// width & height.
						RenderScale.vector_x = FxConfig["render_scale_xy"][0].GetFloat();
						RenderScale.vector_y = FxConfig["render_scale_xy"][1].GetFloat();
					}
					if (RenderScale.vector_x <= 0.0f && RenderScale.vector_y <= 0.0f)
						RenderScale = Vector2T<float>(1.0f, 1.0f);

					Vector2T<uint32_t> VertexHandleTemp = Vector2T<uint32_t>(SFglobalTransmitData.create_vbo_handle(), NULL);
					if (VerAttCreateFlag == NULL)
						ShaderGlobalRes::ResVAO.VaoHandleReg("FxCoreVao", SFglobalTransmitData.create_vao_handle());
					++VerAttCreateFlag;

					VertexHandleTemp.vector_y = ShaderGlobalRes::ResVAO.VaoHandleFind("FxCoreVao");
					ShaderLoader.FxShaderProgramConfig(VertexHandleTemp, COREDATAINFO.WindowSize, FrameRateTime, RenderScale);
					
					// ################################################ UniformLoading ################################################
					// uniform texture(1i), float(if). [2023_12_10]

					vector<FsTextureHandle> CoreDataParamTexture = {};
					// load uniform texture(1i).
					if (FxConfig.HasMember("shader_uniform_texture") && FxConfig["shader_uniform_texture"].IsArray()) {
						for (const auto& cfgitem : FxConfig["shader_uniform_texture"].GetArray()) {

							FsTextureHandle TempTexture = {};
							string TempName = {};

							if (cfgitem.HasMember("image_ufh_name") && cfgitem["image_ufh_name"].IsString())
								TempName = cfgitem["image_ufh_name"].GetString();

							if (cfgitem.HasMember("image_file_path") && cfgitem["image_file_path"].IsString()) {

								FSloadTexture TextureLoader(cfgitem["image_file_path"].GetString());
								TempTexture = TextureLoader.get_texture_handle(ShaderProgramTemp, TempName.c_str());
							}

							// 因为这里要使用"glActiveTexture"所以分配TMU.
							if (!ShaderGlobalRes::ResTMU.TmuCountAllocTex2D(TempTexture))
								PushLogger(LOG_ERR, SYSMD_LB_CORE, "'TMU' not idle.");
							
							CoreDataParamTexture.push_back(TempTexture);
						}
					}

					unordered_map<string, float> CoreDataParamFloat = {};
					// load uniform float.
					if (FxConfig.HasMember("shader_uniform_param") && FxConfig["shader_uniform_param"].IsArray())
						for (const auto& cfgitem : FxConfig["shader_uniform_param"].GetArray())
							CoreDataParamFloat[cfgitem.GetString()] = 1.0f;

					ShaderLoader.FxShaderParamLoader(CoreDataParamTexture, CoreDataParamFloat);
					// core tools load.
					ImProFxTools::FrmCoreOper.FindLoadRenderItem(COREDATAPACK.DataShaderFx, KeyNameTemp, ShaderLoader);
				}
			}
			else
				PushLogger(LOG_WARN, SYSMD_LB_CORE, "unable find core_shaderfx_config.");

			// 加载源纹理(不使用纹理采样器)
			if (config.HasMember("core_raw_texture_config") && config["core_raw_texture_config"].IsArray()) {
				for (const auto& configitem : config["core_raw_texture_config"].GetArray()) {

					FsTextureHandle SrcTextureValue = {};
					string SrcTextureKey = {};

					if (configitem.HasMember("image_unique_name") && configitem["image_unique_name"].IsString())
						SrcTextureKey = configitem["image_unique_name"].GetString();

					if (configitem.HasMember("image_file_path") && configitem["image_file_path"].IsString()) {

						FSloadTexture loadimage(configitem["image_file_path"].GetString());
						SrcTextureValue = loadimage.get_texture_handle(NULL, SrcTextureKey.c_str());
					}
					// hash_map add item.
					COREDATAPACK.DataTextureRaw[SrcTextureKey] = SrcTextureValue;
				}
			}
			else
				PushLogger(LOG_WARN, SYSMD_LB_CORE, "unable find core_raw_texture_config.");
		}
		else
			PushLogger(LOG_ERR, SYSMD_LB_CORE, "shader config object.");
		return return_items;
	}

	// 系统框架初始化.
	bool FrameworkImCore::ProFxCoreInit() {
		bool ReturnFlag = false; // "OR A, NOT B"
		bool FlagsTemp[2] = { false,false };
		
		rapidjson::Document configitem = ReadJsonConfig(DEF_CORE_SYSTEM_CONFIG, FlagsTemp[0]);
		ReturnFlag |= !FlagsTemp[0];
		if (!configitem.HasParseError() && configitem.IsObject()) {

			// window config.
			PushLogger(LOG_INFO, SYSMD_LB_CORE, "opengl window config...");

			uint32_t GLFWversion[2] = { 2,0 };
			if (configitem.HasMember("window_fw_version") && configitem["window_fw_version"].IsArray()) {

				GLFWversion[0] = (uint32_t)configitem["window_fw_version"][0].GetInt();
				GLFWversion[1] = (uint32_t)configitem["window_fw_version"][1].GetInt();
			}

			uint32_t WindowMSAA = 1;
			if (configitem.HasMember("window_MSAA") && configitem["window_MSAA"].IsInt())
				WindowMSAA = (uint32_t)configitem["window_MSAA"].GetInt();

			string WindowName = "";
			if (configitem.HasMember("window_name") && configitem["window_name"].IsString())
				WindowName = configitem["window_name"].GetString();

			Vector2T<float> WindowInitSize = {};
			if (configitem.HasMember("window_size") && configitem["window_size"].IsArray()) {

				WindowInitSize.vector_x = (float)configitem["window_size"][0].GetInt();
				WindowInitSize.vector_y = (float)configitem["window_size"][1].GetInt();
			}

			bool WindowFixedSizeFlag = true;
			if (configitem.HasMember("window_size_fixed") && configitem["window_size_fixed"].IsBool())
				WindowFixedSizeFlag = configitem["window_size_fixed"].GetBool();

			bool WindowFullScreenFlag = true;
			if (configitem.HasMember("window_full_screen") && configitem["window_full_screen"].IsBool())
				WindowFullScreenFlag = configitem["window_full_screen"].GetBool();

			ReturnFlag |= !GLFWwindowInit(GLFWversion, WindowMSAA, false, WindowFixedSizeFlag);
			ReturnFlag |= !GLFWwindowCreate(CoreModuleWindow::WindowConfig(
				(uint32_t)WindowInitSize.vector_x,
				(uint32_t)WindowInitSize.vector_y,
				WindowName,
				WindowFullScreenFlag
			));
			ReturnFlag |= !GLFWwindowVsync(true);
			ReturnFlag |= !GLFWwindowCallback();

			// glfw window icon.
			if (configitem.HasMember("window_icon_file") && configitem["window_icon_file"].IsString())
				GLFWwindowSetIncon(configitem["window_icon_file"].GetString());

			// init 'COREDATAPACK' & 'COREDATAINFO'.
			COREDATAPACK.JsonFileLoader = ReadJsonConfig;
			COREDATAINFO.WindowSize = WindowInitSize;

			// print device info.
			FSloadInformation* devinfo = new FSloadInformation[4]{
				"sysinfo platform gpu",
				"sysinfo memory capacity",
				"sysinfo memory size",
				"sysinfo opengl attribs"
			};
			delete[] devinfo;

			FSloadInformation TexSampleNum("sysinfo opengl texture");
			MaxNumberTMU = (uint32_t)TexSampleNum.get_sizevalue();

			PushLogger(LOG_INFO, SYSMD_LB_CORE, "imgui gui config...");

			string ShaderVersionStr = {}, FontsFilepath = {};
			if (configitem.HasMember("window_sh_version") && configitem["window_sh_version"].IsString())
				ShaderVersionStr = configitem["window_sh_version"].GetString();

			// 默认窗口背景颜色(buffer_color).
			if (configitem.HasMember("window_bg_color") && configitem["window_bg_color"].IsArray()) {
				RenderBgColor.vector_x = configitem["window_bg_color"][0].GetFloat();
				RenderBgColor.vector_y = configitem["window_bg_color"][1].GetFloat();
				RenderBgColor.vector_z = configitem["window_bg_color"][2].GetFloat();
				RenderBgColor.vector_w = configitem["window_bg_color"][3].GetFloat();
			}

			// 默认背景着色器.
			if (configitem.HasMember("window_bg_shader") && configitem["window_bg_shader"].IsBool()) {
				if (configitem["window_bg_shader"].GetBool()) {
					BackgroundFunction =
						[&]() {
						ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
						ImGuiPro::FullScreenBackground(ImProFxTools::FrmCore.FindRenderItemFBO(COREDATAPACK.DataShaderFx, "fx_background_shader"), false);
						ImGui::PopStyleColor();
					};
					LOGCONS::PushLogger(LOG_INFO, SYSMD_LB_CORE, "enable default background_shaders.");
				}
			}

			if (configitem.HasMember("imgui_fonts_file") && configitem["imgui_fonts_file"].IsString())
				FontsFilepath = configitem["imgui_fonts_file"].GetString();

			Vector4T<float> GlobalTextColor = {};
			if (configitem.HasMember("imgui_fonts_color") && configitem["imgui_fonts_color"].IsArray()) {

				GlobalTextColor.vector_x = configitem["imgui_fonts_color"][0].GetFloat();
				GlobalTextColor.vector_y = configitem["imgui_fonts_color"][1].GetFloat();
				GlobalTextColor.vector_z = configitem["imgui_fonts_color"][2].GetFloat();
				GlobalTextColor.vector_w = configitem["imgui_fonts_color"][3].GetFloat();
			}

			float FontsGlobalSize = 16.0f, FrameRounding = 0.0f, WindowRounding = 0.0f;
			if (configitem.HasMember("imgui_fonts_size") && configitem["imgui_fonts_size"].IsFloat())
				FontsGlobalSize = configitem["imgui_fonts_size"].GetFloat();

			if (configitem.HasMember("imgui_window_rounding") && configitem["imgui_window_rounding"].IsFloat())
				WindowRounding = configitem["imgui_window_rounding"].GetFloat();

			if (configitem.HasMember("imgui_frame_rounding") && configitem["imgui_frame_rounding"].IsFloat())
				FrameRounding = configitem["imgui_frame_rounding"].GetFloat();

			ImGuiInit(MainWindowObject,
				CoreModuleImGui::GuiConfig(
					ShaderVersionStr,
					FontsFilepath,
					GlobalTextColor,
					FontsGlobalSize,
					FrameRounding,
					WindowRounding
				)
			);

			// load shader config file => load shader.
			if (configitem.HasMember("shader_reg_file") && configitem["shader_reg_file"].IsString())
				GraphicsLoadingConfig(ReadJsonConfig(configitem["shader_reg_file"].GetString(), FlagsTemp[1]));
			ReturnFlag |= !FlagsTemp[1];

			// create thread pool.
			if (configitem.HasMember("system_create_workers") && configitem["system_create_workers"].IsUint())
				COREDATAPACK.TaskThreadPool = new ImThreadTask::ThreadingPool(configitem["system_create_workers"].GetUint());

			// init user_object logic.
			for (auto it = COREDATAUSEROBJ.begin(); it != COREDATAUSEROBJ.end(); ++it)
				ReturnFlag |= !it->second->LogicInitialization(COREDATAPACK);
		}
		else
			PushLogger(LOG_ERR, SYSMD_LB_CORE, "system config object.");

		return !ReturnFlag;
	}

	void FrameworkImCore::ResourcesPackageFree(FRMCORE_PACKAGE& package) {
		// -------------------------------- [DATA] --------------------------------
		// free shader resources.
		for (auto it = package.DataShaderFx.begin(); it != package.DataShaderFx.end(); ++it)
			system_tools::DeleteFxShaderItem(it->second);
		PushLogger(LOG_INFO, SYSMD_LB_CORE, "free resources shader_package.");

		// free raw texture resources.
		for (auto it = package.DataTextureRaw.begin(); it != package.DataTextureRaw.end(); ++it)
			SFglobalDeletel.delete_texture_handle(it->second);
		PushLogger(LOG_INFO, SYSMD_LB_CORE, "free resources texture_handle.");

		// free animation resources.
		for (auto it = package.DataAnimation.begin(); it != package.DataAnimation.end(); ++it)
			if (it->second != nullptr)
				delete it->second;

		PushLogger(LOG_INFO, SYSMD_LB_CORE, "free resources animation_object.");

		// free memory resources.
		for (auto it = package.DataDynamicMem.begin(); it != package.DataDynamicMem.end(); ++it)
			if (it->second != nullptr)
				delete it->second;

		PushLogger(LOG_INFO, SYSMD_LB_CORE, "free resources memory_object.");

		// -------------------------------- [STATIC] --------------------------------
		// free flag resources.
		ImProFxTools::FrmStatic.StaticFlagDelete(package.StaticFlag, "#", STCMODE_DELETE_ALL);
		PushLogger(LOG_INFO, SYSMD_LB_CORE, "free resources flag_object.");

		// free timer resources.
		ImProFxTools::FrmStatic.StaticTimerDelete(package.StaticTimer, "#", STCMODE_DELETE_ALL);
		PushLogger(LOG_INFO, SYSMD_LB_CORE, "free resources timer_object.");

		// free particle_system resources.
		ImProFxTools::FrmStatic.StaticParticleSysDelete(package.ParticleSystem, "#", STCMODE_DELETE_ALL);
		PushLogger(LOG_INFO, SYSMD_LB_CORE, "free resources particle_system_object.");

		Vector2T<size_t> temp_param = {};
		if (ImProFxTools::FrmStatic.GetOperInfo(temp_param))
			PushLogger(LOG_INFO, SYSMD_LB_CORE, "trace operations 'static' not_error.");
		else
			PushLogger(LOG_WARN, SYSMD_LB_CORE, "trace operations 'static' alloc: %u delete: %u", temp_param.vector_x, temp_param.vector_y);

		// delete thread_pool object.
		delete package.TaskThreadPool;
	}

	bool FrameworkImCore::ProFxCoreFree() {
		bool ReturnFlag = false;

		// delete resources hash_map.
		ResourcesPackageFree(COREDATAPACK);
		// delete global vertex_attribute.
		ShaderGlobalRes::ResVAO.VaoHandleFree();
		/*
		// delete test pointer.
		if ((xxx*)COREDATAINFO._DeveloperTestPointer != nullptr)
			delete (xxx*)COREDATAINFO._DeveloperTestPointer;
		*/

		// delete user object(register).
		for (auto it = COREDATAUSEROBJ.begin(); it != COREDATAUSEROBJ.end(); ++it)
			if (it->second != nullptr) {
				// free logic.
				string StrKeyTemp = it->first;
				it->second->LogicCloseFree();

				// 防止一些吊毛在"LogicCloseFree()"里用反射删对象. /滑稽
				if (COREDATAUSEROBJ.find(StrKeyTemp) != COREDATAUSEROBJ.end()) {
					const string* StrNameTemp = &it->first;
					delete it->second;
					PushLogger(LOG_TRC, SYSMD_LB_CORE, "free user reg_obejct: %s", StrNameTemp->c_str());
				}
				else
					PushLogger(LOG_WARN, SYSMD_LB_CORE, "failed: free user reg_obejct 'LogicCloseFree' => nullptr: %s", it->first.c_str());
			}
			else
				PushLogger(LOG_ERR, SYSMD_LB_CORE, "failed: free user reg_obejct = nullptr: %s", it->first.c_str());

		ImGuiFree();
		ReturnFlag |= !GLFWwindowFree();

		return !ReturnFlag;
	}
}