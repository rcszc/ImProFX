// framework_core_system.
#include "framework_core.hpp"

using namespace std;
using namespace LOGCONS;

namespace CoreModuleSystem {
	CoreModuleIMFX::FrameworkImCore* FrameworkCoreGlobalObject = nullptr;

	int32_t ProfxSystemFramework::StartProfx() {
		int32_t ReturnLoopFlag = 1;
		int32_t ReturnMainFlag = NULL;

		// start logger process (默认框架系统文件夹).
		LOGFILE::StartLogFileProcess(FrameworkSystemPath[0]);
		// register_user class,object.
		InitialStageRegUserClass();

		if (FrameworkCoreGlobalObject->ProFxCoreInit(FrameworkConfigFilePath)) {
			// framework main event_loop.
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
			LOGCONS::PushLogger(LOG_ERROR, "[reflection]: ", "failed: class not found in the registry.");
	}

	void ProfxReflection::ReflectionObjectDelete(const char* UniqueName) {
		FrameworkCoreGlobalObject->UserDeleteObject(UniqueName);
	}
}

CoreModuleSystem::ProfxSystemFramework IMFX_SYSTEM_OBJ;
CoreModuleSystem::ProfxReflection      IMFXREF;

namespace CoreModuleIMFX {

	rapidjson::Document FrameworkImCore::JsonConfigLoader(const string& file, bool& state) {
		rapidjson::Document JsonDocument;

		ifstream ReadJsonFile(file);
		if (ReadJsonFile.is_open()) {
			// read char file.
			string JsonDocString((istreambuf_iterator<char>(ReadJsonFile)), istreambuf_iterator<char>());
			ReadJsonFile.close();

			JsonDocument.Parse(JsonDocString.c_str());
			PushLogger(LOG_INFO, SYSMD_LB_CORE, "loader: read json config file: %s", file.c_str());
			state = true;
		}
		else {
			PushLogger(LOG_WARNING, SYSMD_LB_CORE, "loader: failed read json config.");
			state = false;
		}
		return JsonDocument;
	}
	
	/*
	* "LLRES_xxx" low.layer resource, key_name_fmt: IMPROFX_GEN_NUMBER
	* key_name_format update: 2024.01.25 RCSZ.
	* 图形加载&配置.
	*/
	size_t FrameworkImCore::GraphicsLoadingConfig(rapidjson::Document config) {
		size_t ReturnItemsCount = NULL;
		// VAO create flag(count).
		uint32_t VerAttCreateFlag = NULL;
		uint32_t VerBuffer = NULL;

		IMPROFX_GEN_NUMBER GenResID;
		auto GenResourceID = [&]() { return to_string(GenResID.IPFGEN_GenNumberTime()); };

		// 加载初始 着色器纹理资源.
		if (!config.HasParseError() && config.IsObject()) {
			if (config.HasMember("core_shaderfx_config") && config["core_shaderfx_config"].IsArray()) {
				for (const auto& FxConfig : config["core_shaderfx_config"].GetArray()) {

					std::string PackageNameTemp = {};
					FxRenderGroup PackageRenderItem = {};

					if (FxConfig.HasMember("shaderfx_package") && FxConfig["shaderfx_package"].IsString())
						PackageNameTemp = FxConfig["shaderfx_package"].GetString();
					
					// ******************************** shader_script ********************************
					
					IMFX_OGL_MAG::ImFXOGLshader ShadersProcess;

					if (FxConfig.HasMember("shader_file_vertex") && FxConfig["shader_file_vertex"].IsString())
						ShadersProcess.ShaderLoaderVS(FxConfig["shader_file_vertex"].GetString());

					if (FxConfig.HasMember("shader_file_fragment") && FxConfig["shader_file_fragment"].IsString())
						ShadersProcess.ShaderLoaderFS(FxConfig["shader_file_fragment"].GetString());

					if (ShadersProcess.CreateCompileShaders()) {
						// package main_index name.
						PackageRenderItem.ShaderProgramItem = PackageNameTemp;
						LLRES_Shaders->ResourceStorage(PackageRenderItem.ShaderProgramItem, &ShadersProcess);
					}

					// ******************************** frame_buffer ********************************
					// create texture => storage => create fbo, bind fbo => storage. 

					IMFX_OGL_MAG::ImFXOGLtexture FboTextureProcess;
					IMFX_OGL_MAG::ImFXOGLframebuffer FboProcess;

					PackageRenderItem.FrameBufferTexItem = GenResourceID();
					PackageRenderItem.FrameBufferItem    = GenResourceID();

					if (FboTextureProcess.CreateTextureEmpty((uint32_t)COREDATAINFO.WindowSize.vector_x, (uint32_t)COREDATAINFO.WindowSize.vector_y, 4, LinearFiltering))
						LLRES_Textures->ResourceStorage(PackageRenderItem.FrameBufferTexItem, &FboTextureProcess);

					if (FboProcess.CreateBindTexture(LLRES_Textures->ResourceFind(PackageRenderItem.FrameBufferTexItem)))
						LLRES_FrameBuffers->ResourceStorage(PackageRenderItem.FrameBufferItem, &FboProcess);

					// ******************************** render_param ********************************
					// render scale,mvp_matrix,fps.

					if (FxConfig.HasMember("render_max_fps") && FxConfig["render_max_fps"].IsFloat())
						PackageRenderItem.RenderRateTime = uint64_t(1000.0f / FxConfig["render_max_fps"].GetFloat() * 1000.0f);
					
					Vector2T<float> RenderScale = {};
					if (FxConfig.HasMember("render_scale_xy") && FxConfig["render_scale_xy"].IsArray()) {
						// width & height.
						RenderScale.vector_x = FxConfig["render_scale_xy"][0].GetFloat();
						RenderScale.vector_y = FxConfig["render_scale_xy"][1].GetFloat();
					}
					if (RenderScale.vector_x <= 0.0f && RenderScale.vector_y <= 0.0f)
						RenderScale = Vector2T<float>(1.0f, 1.0f);
					
					float OrthoCubeSacle = RenderScale.vector_x / RenderScale.vector_y;
					float OrthoCubeLength = 10.0f;

					glm::mat4 ProjectionMatrix = glm::ortho(
						-OrthoCubeLength * OrthoCubeSacle, OrthoCubeLength * OrthoCubeSacle,
						-OrthoCubeLength, OrthoCubeLength, -OrthoCubeLength, OrthoCubeLength
					);

					// convert: glm matrix => imfx matrix.
					const float* glmmatptr = glm::value_ptr(ProjectionMatrix);
					memcpy_s(PackageRenderItem.RenderMatrixMvp.matrix, 16 * sizeof(float), glmmatptr, 16 * sizeof(float));

					// ******************************** render_model ********************************
					// template rect(triangle * 2).

					IMFX_OGL_MAG::ImFXOGLmodel ModelProcess;

					auto VerBufferTemp = ModelProcess.CreateVertexBuffer();

					if (VerAttCreateFlag == NULL)
						LLRES_VertexAttributes->ResourceStorage("VER_ATTR1", ModelProcess.CreateVertexAttribute(0, 0));
					++VerAttCreateFlag;

					if (ModelProcess.CreateStaticModel(
						LLRES_VertexAttributes->ResourceFind("VER_ATTR1"),
						VerBufferTemp,
						IMFX_OGL_MAG::ShaderTemplateRect,
						IMFX_OGL_MAG::ShaderTemplateRectLen * sizeof(float)
					)) {
						PackageRenderItem.ModelItem = GenResourceID();
						LLRES_VertexBuffers->ResourceStorage(PackageRenderItem.ModelItem, &ModelProcess);
					}
					
					// ******************************** uniform_texture ********************************
					// uniform texture => sampler.

					vector<string> FxDataParamTexture = {};
					// load uniform texture(1i).
					if (FxConfig.HasMember("shader_uniform_texture") && FxConfig["shader_uniform_texture"].IsArray()) {
						for (const auto& cfgitem : FxConfig["shader_uniform_texture"].GetArray()) {

							IMFX_OGL_MAG::ImFXOGLtexture UTextureProcess;
							string TextureName = {};

							if (cfgitem.HasMember("image_ufh_name") && cfgitem["image_ufh_name"].IsString())
								TextureName = cfgitem["image_ufh_name"].GetString();

							if (cfgitem.HasMember("image_file_path") && cfgitem["image_file_path"].IsString()) {
								// 因为这里要传入着色器,所以分配'TMU'.
								UTextureProcess.TextureSamplerUnit = LLRES_Samplers->AllocTmuCount();

								// uniform: storage_name = srouce_name.
								if (UTextureProcess.CreateTextureLoader(cfgitem["image_file_path"].GetString(), LinearFiltering))
									LLRES_Textures->ResourceStorage(TextureName, &UTextureProcess);
							}
							FxDataParamTexture.push_back(TextureName);
						}
					}
					PackageRenderItem.ParamTextureItems = FxDataParamTexture;

					// ******************************** uniform_float ********************************
					// uniform 1f,float32.

					unordered_map<string, float> FxDataParamFloat = {};
					// load uniform float.
					if (FxConfig.HasMember("shader_uniform_param") && FxConfig["shader_uniform_param"].IsArray())
						for (const auto& cfgitem : FxConfig["shader_uniform_param"].GetArray())
							FxDataParamFloat[cfgitem.GetString()] = 1.0f;

					PackageRenderItem.ParamValueFloat = FxDataParamFloat;

					// ******************************** storage_core ********************************
					// system loader => core_data.
					PackageRenderItem.ExecuteProgramFlag = true;

					auto it = COREDATAPACK.DataShaderFx.find(PackageNameTemp);
					if (it != COREDATAPACK.DataShaderFx.end())
						PushLogger(LOG_ERROR, SYSMD_LB_CORE, "failed create fx_shader duplicate_key: %s", PackageNameTemp.c_str());
					else {
						// 动态背景着色器单独管理.
						if (PackageNameTemp == "system_background_shader")
							COREDATAPACK.BackgroundShaderFX = PackageRenderItem;
						else
							COREDATAPACK.DataShaderFx[PackageNameTemp] = PackageRenderItem;
						PushLogger(LOG_TRACE, SYSMD_LB_CORE, "create fx_shader key: %s", PackageNameTemp.c_str());
					}
				}
			}
			else
				PushLogger(LOG_WARNING, SYSMD_LB_CORE, "unable find core_shaderfx_config.");

			// 加载初始 源纹理.
			if (config.HasMember("core_raw_texture_config") && config["core_raw_texture_config"].IsArray()) {
				for (const auto& SrctexConfig : config["core_raw_texture_config"].GetArray()) {

					IMFX_OGL_MAG::ImFXOGLtexture STextureProcess;
					string SrcTextureKey   = {};
					string SrcTextureValue = {};

					if (SrctexConfig.HasMember("image_unique_name") && SrctexConfig["image_unique_name"].IsString())
						SrcTextureKey = SrctexConfig["image_unique_name"].GetString();

					SrcTextureValue = GenResourceID();

					if (SrctexConfig.HasMember("image_file_path") && SrctexConfig["image_file_path"].IsString()) {

						if (STextureProcess.CreateTextureLoader(SrctexConfig["image_file_path"].GetString(), LinearFiltering))
							LLRES_Textures->ResourceStorage(SrcTextureValue, &STextureProcess);
					}
					// hash_map add item.
					COREDATAPACK.DataTextureRaw[SrcTextureKey] = SrcTextureValue;
				}
			}
			else
				PushLogger(LOG_WARNING, SYSMD_LB_CORE, "unable find core_raw_texture_config.");
		}
		else
			PushLogger(LOG_ERROR, SYSMD_LB_CORE, "shader config object.");

		// ******************************** 最后处理阶段(GUI)着色器 ********************************.
		IMFX_OGL_MAG::ImFXOGLshader ShaderProcess;

		ShaderProcess.ShaderLoaderVS(FrameworkSystemPath[4]);
		ShaderProcess.ShaderLoaderFS(FrameworkSystemPath[5]);

		if (ShaderProcess.CreateCompileShaders()) {
			// gui_final shader.
			GuiFxProcess.ShaderProgramItem = GenResourceID();
			LLRES_Shaders->ResourceStorage(GuiFxProcess.ShaderProgramItem, &ShaderProcess);
		}

		IMFX_OGL_MAG::ImFXOGLmodel ModelProcess;

		if (ModelProcess.CreateStaticModel(
			LLRES_VertexAttributes->ResourceFind("VER_ATTR1"),
			ModelProcess.CreateVertexBuffer(),
			IMFX_OGL_MAG::ShaderTemplateRect,
			IMFX_OGL_MAG::ShaderTemplateRectLen * sizeof(float)
		)) {
			GuiFxProcess.ModelItem = GenResourceID();
			LLRES_VertexBuffers->ResourceStorage(GuiFxProcess.ModelItem, &ModelProcess);
		}

		glm::mat4 ProjectionMatrix = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 10.0f);
		// convert: glm matrix => imfx matrix.
		const float* glmmatptr = glm::value_ptr(ProjectionMatrix);
		memcpy_s(GuiFxProcess.RenderMatrixMvp.matrix, 16 * sizeof(float), glmmatptr, 16 * sizeof(float));

		IMFX_OGL_MAG::ImFXOGLtexture FboTextureProcess;
		IMFX_OGL_MAG::ImFXOGLframebuffer FboProcess;

		// frame_buffer =bind=> texture.
		GuiFxProcess.FrameBufferProcessTex = GenResourceID();
		GuiFxProcess.FrameBufferItem       = GenResourceID();

		FboTextureProcess.TextureSamplerUnit = LLRES_Samplers->AllocTmuCount();

		if (FboTextureProcess.CreateTextureEmpty((uint32_t)COREDATAINFO.WindowSize.vector_x, (uint32_t)COREDATAINFO.WindowSize.vector_y, 4, LinearFiltering))
			LLRES_Textures->ResourceStorage(GuiFxProcess.FrameBufferProcessTex, &FboTextureProcess);

		if (FboProcess.CreateBindTexture(LLRES_Textures->ResourceFind(GuiFxProcess.FrameBufferProcessTex)))
			LLRES_FrameBuffers->ResourceStorage(GuiFxProcess.FrameBufferItem, &FboProcess);

		PushLogger(LOG_TRACE, SYSMD_LB_CORE, "gui_final shader_package loading completed.");
		return ReturnItemsCount;
	}

	// 系统框架初始化.
	bool FrameworkImCore::ProFxCoreInit(const char* config_file) {
		bool ReturnFlag = false; // "OR A, NOT B"
		bool FlagsTemp[2] = { false,false };
		
		rapidjson::Document configitem = JsonConfigLoader(config_file, FlagsTemp[0]);
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

			// ******************************** 'COREDATAPACK' & 'COREDATAINFO' ********************************
			
			COREDATAPACK.JsonFileLoader = JsonConfigLoader;
			COREDATAINFO.S_UniqueCountID = &SYSIMGUI_COUPLING::ImGuiControlUniqueIDcount;
			COREDATAINFO.WindowSize     = WindowInitSize;

			// create thread pool.
			if (configitem.HasMember("system_create_workers") && configitem["system_create_workers"].IsUint())
				COREDATAPACK.TaskThreadPool = new ImThreadTask::WorkPool(configitem["system_create_workers"].GetUint());

			// create core_gui(imgui). (new object)
			COREDATAINFO.ImProFXGUI = new ImProFXgui::FrameworkGUI(&SYSIMGUI_COUPLING::ImGuiControlUniqueIDcount);

			// ******************************** Graphical ********************************
			// init renderer, => logger process function.
			LLMAG_RendererInit.RendererInit();
			LLMAG_RendererInit.LoggerFunc(PushLogger);
			
			// get device information.
			NST_ImFxOGLinformation PlatformDeviceInfo;
			PlatformDeviceInfo.LoggerPrintInformation = true;

			PlatformDeviceInfo.InfoGetVertexUnitsMax();

			// get device tmu_max => framework core.
			DeviceTmuMax = (uint32_t)PlatformDeviceInfo.InfoGetTextureUnitsMax();

			PlatformDeviceInfo.InfoGetGPUmemoryCapacity();
			PlatformDeviceInfo.InfoGetGPUmemoryUsage();
			PlatformDeviceInfo.InfoPrintPlatformParameters();

			// LLRES create init objects. param2: TMU max_number.
			LowLevelResourceCreate(LOGCONS::PushLogger, 32);

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

					COREDATAPACK.BackgroundShaderFX.ExecuteProgramFlag = true;
					BackgroundFunction =
						[&](ImfxShader BgShHD) {
						ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
						ImGuiPro::FullScreenImWindowBg(IMTOOL_CVT_IMPTR(BgShHD), true);
						ImGui::PopStyleColor();
					};
					LOGCONS::PushLogger(LOG_INFO, SYSMD_LB_CORE, "enable default background_shaders.");
				}
			}

			if (configitem.HasMember("imgui_fonts_file") && configitem["imgui_fonts_file"].IsString())
				FontsFilepath = configitem["imgui_fonts_file"].GetString();

			Vector4T<float> FontsTextColor = {};
			if (configitem.HasMember("imgui_fonts_color") && configitem["imgui_fonts_color"].IsArray()) {

				FontsTextColor.vector_x = configitem["imgui_fonts_color"][0].GetFloat();
				FontsTextColor.vector_y = configitem["imgui_fonts_color"][1].GetFloat();
				FontsTextColor.vector_z = configitem["imgui_fonts_color"][2].GetFloat();
				FontsTextColor.vector_w = configitem["imgui_fonts_color"][3].GetFloat();
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
					ShaderVersionStr, // opengl glsl_script:"#version xxx core", vulkan...
					FontsFilepath,    // imgui global_style fonts file_path'.ttf'
					FontsTextColor,   // imgui global_style text color.
					FontsGlobalSize,  // imgui global_style text scale.
					FrameRounding,    // imgui global_style frame(graphical) rounding.
					WindowRounding    // imgui global_style window rounding.
				)
			);

			// load shader config file => load shader.
			if (configitem.HasMember("shader_reg_file") && configitem["shader_reg_file"].IsString())
				GraphicsLoadingConfig(JsonConfigLoader(configitem["shader_reg_file"].GetString(), FlagsTemp[1]));
			ReturnFlag |= !FlagsTemp[1];

			// init user_object logic.
			for (auto it = COREDATAUSEROBJ.begin(); it != COREDATAUSEROBJ.end(); ++it)
				ReturnFlag |= !it->second->LogicInitialization(COREDATAPACK);
		}
		else
			PushLogger(LOG_ERROR, SYSMD_LB_CORE, "system config object.");
		return !ReturnFlag;
	}

	void FrameworkImCore::ResourcesPackageFree(FRMCORE_PACKAGE& package) {
		// ******************************** [STATIC] ********************************
		// free flag resources.
		ImProFxTools::FrmModule.FlagModuleDelete(package.StaticFlag, "", STCMODE_DELETE_ALL);
		PushLogger(LOG_INFO, SYSMD_LB_CORE, "free resources flag_objects.");

		// free timer resources.
		ImProFxTools::FrmModule.TimerModuleDelete(package.StaticTimer, "", STCMODE_DELETE_ALL);
		PushLogger(LOG_INFO, SYSMD_LB_CORE, "free resources timer_object.");

		// free particle_system resources.
		ImProFxTools::FrmModule.ParticleModuleDelete(package.ParticleSystem, "", STCMODE_DELETE_ALL);
		PushLogger(LOG_INFO, SYSMD_LB_CORE, "free resources particle_system_object.");

		Vector2T<size_t> temp_param = {};
		// flag,timer,particle_system oper count.
		if (!ImProFxTools::FrmModule.GetOperInfo(temp_param))
			PushLogger(LOG_WARNING, SYSMD_LB_CORE, "trace operations 'static' alloc: %u delete: %u", temp_param.vector_x, temp_param.vector_y);

		// delete thread_pool object.
		delete package.TaskThreadPool;
	}

	void FrameworkImCore::ResourcesInfoFree(FRMCORE_INFO& dyinfo) {
		delete dyinfo.ImProFXGUI;
	}

	bool FrameworkImCore::ProFxCoreFree() {
		bool ReturnFlag = false;

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
					PushLogger(LOG_TRACE, SYSMD_LB_CORE, "free ref_object: %s", StrNameTemp->c_str());
				}
				else
					PushLogger(LOG_WARNING, SYSMD_LB_CORE, "failed: free ref_object 'LogicCloseFree' => nullptr: %s", it->first.c_str());
			}
			else
				PushLogger(LOG_ERROR, SYSMD_LB_CORE, "failed: free ref_object = nullptr: %s", it->first.c_str());

		// delete resources data.
		ResourcesPackageFree(COREDATAPACK);
		ResourcesInfoFree(COREDATAINFO);
		// LLRES free objects.
		ReturnFlag |= !LowLevelResourceFree();

		/*
		// delete test pointer.
		if ((xxx*)COREDATAINFO._DeveloperTestPointer != nullptr)
			delete (xxx*)COREDATAINFO._DeveloperTestPointer;
		*/
		// UserClass => Package => Info => LLRES => ImGui => GLFW.
		ImGuiFree();
		ReturnFlag |= !GLFWwindowFree();

		return !ReturnFlag;
	}
}