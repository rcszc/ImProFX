// framework_core_system.
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "framework_core.hpp"

using namespace std;
using namespace LOGCONS;

namespace CoreModuleSystem {
	CoreModuleIMFX::FrameworkImCore* FrameworkCoreGlobalObject = nullptr;

	int32_t ProfxSystemFramework::StartProfx() {
		int32_t loopreturnflag = 1;
		int32_t mainreturnflag = NULL;

		// start log process (默认框架系统文件夹).
		LOGFILE::StartLogFileProcess("imgui_profx_system/syslog/");

		if (FrameworkCoreGlobalObject->ProfxInit()) {

			while (loopreturnflag == 1) {
				loopreturnflag = FrameworkCoreGlobalObject->ProfxEventLoop();
			}
			if (loopreturnflag == -1)
				mainreturnflag = STAGE_EXIT_LOOP;

			if (!FrameworkCoreGlobalObject->ProfxFree())
				mainreturnflag = STAGE_EXIT_FREE;

			delete FrameworkCoreGlobalObject;
		}
		else
			mainreturnflag = STAGE_EXIT_INIT;

		LOGFILE::FreeLogFileProcess(); // free process thread.
		return mainreturnflag;
	}

	void ProfxReflection::ReflectionObjectCreate(const char* ClassName, const char* ObjectName) {
		auto& registry = ReflectionFactory::GetRegistry();

		auto it = registry.find(ClassName);
		if (it != registry.end())
			FrameworkCoreGlobalObject->UserRegistrationObject(ClassName, ObjectName, it->second());
		else
			PushLogger(LOG_ERR, "[reflection]: ", "failed: class not found in the registry.");
	}

	void ProfxReflection::ReflectionObjectDelete(const char* UniqueObjectName) {
		FrameworkCoreGlobalObject->UserDeleteObject(UniqueObjectName);
	}
}

CoreModuleSystem::ProfxSystemFramework IMFX_SYSTEM_OBJ;
CoreModuleSystem::ProfxReflection      IMFXREF;

namespace CoreModuleIMFX {

	rapidjson::Document FrameworkImCore::ReadJsonConfig(const string& file, bool& state) {
		rapidjson::Document configdoc;

		ifstream read_jsonfile(file);
		if (read_jsonfile.is_open()) {
			// read char file.
			string json_string((istreambuf_iterator<char>(read_jsonfile)), istreambuf_iterator<char>());
			read_jsonfile.close();

			configdoc.Parse(json_string.c_str());
			PushLogger(LOG_INFO, SYSMD_LB_CORE, "read json config file: %s", file.c_str());
			state = true;
		}
		else {
			PushLogger(LOG_WARN, SYSMD_LB_CORE, "failed read json config.");
			state = false;
		}
		return configdoc;
	}
	
	// 加载着色器配置.
	size_t FrameworkImCore::ShaderConfigLoad(rapidjson::Document config) {
		size_t return_items = NULL;
		// VAO create flag(count).
		uint32_t vertex_buffer_count = NULL;

		// set vertex attribute count_begin.
		SFglobalTransmitData.set_location_begin(0);

		if (!config.HasParseError() && config.IsObject()) {
			if (config.HasMember("core_shaderfx_config") && config["core_shaderfx_config"].IsArray()) {
				uint32_t vao_tmep = {};
				// item / items.
				for (const auto& configitem : config["core_shaderfx_config"].GetArray()) {
					
					FxRenderItem ItemDataTemp = {}; // shader(包).
					string KeyNameTemp = {};

					if (configitem.HasMember("shaderfx_package") && configitem["shaderfx_package"].IsString())
						KeyNameTemp = configitem["shaderfx_package"].GetString();
					PushLogger(LOG_TRC, SYSMD_LB_CORE, "shader config item_name[key]: %s", KeyNameTemp.c_str());

					// 加载着色器 vs, fs.
					FSloadShader load_shader;

					if (configitem.HasMember("shader_file_vertex") && configitem["shader_file_vertex"].IsString())
						load_shader.load_shader_vs(configitem["shader_file_vertex"].GetString());

					if (configitem.HasMember("shader_file_fragment") && configitem["shader_file_fragment"].IsString())
						load_shader.load_shader_fs(configitem["shader_file_fragment"].GetString());
					
					ItemDataTemp.ShaderProgramHandle = load_shader.create_program_handle();
					load_shader.delete_shader();

					// 获取渲染缩放(分辨率).
					Vector2T<float> Resolution = {};
					if (configitem.HasMember("render_scale_xy") && configitem["render_scale_xy"].IsArray()) {

						Resolution.vector_x = configitem["render_scale_xy"][0].GetFloat();
						Resolution.vector_y = configitem["render_scale_xy"][1].GetFloat();
					}
					if (Resolution.vector_x == -1 && Resolution.vector_y == -1)
						Resolution = Vector2T<float>(1.0f, 1.0f);

					Resolution.vector_x = abs(Resolution.vector_x);
					Resolution.vector_y = abs(Resolution.vector_y);

					// 正交渲染缩放.
					float RenderScale = Resolution.vector_x / Resolution.vector_y;

					// 创建 FBO 附着纹理. 离屏渲染 =FBO=> 纹理.
					FScreateTexture CreateTexture(TextureColorRGBA, COREDATAINFO.WindowSize);
					FSloadFrameBuffer CreateFBO;
					CreateFBO.create_bind_framebuffer(CreateTexture.get_texture_handle());

					ItemDataTemp.FrameBufferTex = FsTextureHandle(CreateTexture.get_texture_handle(), ItemDataTemp.ShaderProgramHandle, NULL, "FBTX");
					ItemDataTemp.FrameBufferHandle = CreateFBO.get_framebuffer_handle();

					CreateTexture.unbind_texture();

					float MatrixOrtho = 10.0f;
					// shader projection matrix.
					glm::mat4 ProjectionMatrix = glm::ortho(
						-MatrixOrtho * RenderScale, MatrixOrtho * RenderScale, -MatrixOrtho, MatrixOrtho, -MatrixOrtho, MatrixOrtho
					);
					// glm matrix => fp32_pointer.
					const float* glmmatptr = glm::value_ptr(ProjectionMatrix);
					memcpy_s(ItemDataTemp.RenderMatrixMvp.matrix, 16 * sizeof(float), glmmatptr, 16 * sizeof(float));

					// 最大渲染帧率.
					int64_t rate_time = NULL;
					if (configitem.HasMember("render_max_fps") && configitem["render_max_fps"].IsInt64())
						rate_time = configitem["render_max_fps"].GetInt64();

					// 计时(单位:微秒): (now - 'RenderRateTimer') > 'RenderRateTime' ? : render
					ItemDataTemp.RenderRateTime = 1000 / rate_time * 1000;

					// load vertex data(triangle).
					FsVertexGroups ModelDataTemp = FsVertexGroups(ShaderTemplateRectLen, ShaderTemplateRect);

					uint32_t vbo_tmep = SFglobalTransmitData.create_vbo_handle();
					// 先创建 vbo 句柄对象.
					if (vertex_buffer_count == 0) {
						// create => temp => global reg.
						vao_tmep = SFglobalTransmitData.create_vao_handle();
						ShaderGlobalRes::ResVAO.VaoHandleReg("FxCoreVao", vao_tmep);
					}
					
					// create transmit model_data.
					SFglobalTransmitData.load_vertex_static(vao_tmep, vbo_tmep, ModelDataTemp, ItemDataTemp.ModelHandle);
					++vertex_buffer_count;

					// load texture array.
					if (configitem.HasMember("shader_uniform_texture") && configitem["shader_uniform_texture"].IsArray()) {
						for (const auto& cfgitem : configitem["shader_uniform_texture"].GetArray()) {

							FsTextureHandle TempTexture = {};
							string TempName = {};

							if (cfgitem.HasMember("image_ufh_name") && cfgitem["image_ufh_name"].IsString())
								TempName = cfgitem["image_ufh_name"].GetString();

							if (cfgitem.HasMember("image_file_path") && cfgitem["image_file_path"].IsString()) {

								FSloadTexture loadimage(cfgitem["image_file_path"].GetString());
								TempTexture = loadimage.get_texture_handle(ItemDataTemp.ShaderProgramHandle, TempName.c_str());
							}

							// 因为这里要使用"glActiveTexture"所以分配TMU.
							if (!ShaderGlobalRes::ResTMU.TmuCountAllocTex2D(TempTexture))
								PushLogger(LOG_ERR, SYSMD_LB_CORE, "'TMU' not idle.");
							
							ItemDataTemp.ParamTexture.push_back(TempTexture);
						}
					}

					// load uniform float.
					if (configitem.HasMember("shader_uniform_param") && configitem["shader_uniform_param"].IsArray())
						for (const auto& cfgitem : configitem["shader_uniform_param"].GetArray())
							ItemDataTemp.ParamValueFloat[cfgitem.GetString()] = 1.0f;

					ItemDataTemp.ExecuteProgramFlag = true;
					// hash_map add item.
					COREDATAPACK.DataShaderFx[KeyNameTemp] = ItemDataTemp;
				}
			}
			else
				PushLogger(LOG_WARN, SYSMD_LB_CORE, "unable find core_shaderfx_config.");

			// 加载源贴图(不使用纹理采样器)
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
	bool FrameworkImCore::ProfxInit() {
		bool return_initflag = false; // "OR A, NOT B"
		bool temp_flag[2] = { false,false };
		
		rapidjson::Document configitem = ReadJsonConfig(DEF_CORE_SYSTEM_CONFIG, temp_flag[0]);
		return_initflag |= !temp_flag[0];
		if (!configitem.HasParseError() && configitem.IsObject()) {

			// window config.
			PushLogger(LOG_INFO, SYSMD_LB_CORE, "opengl window config...");

			uint32_t fw_version[2] = { 2,0 };
			if (configitem.HasMember("window_fw_version") && configitem["window_fw_version"].IsArray()) {

				fw_version[0] = (uint32_t)configitem["window_fw_version"][0].GetInt();
				fw_version[1] = (uint32_t)configitem["window_fw_version"][1].GetInt();
			}

			uint32_t window_MSAA = 1;
			if (configitem.HasMember("window_MSAA") && configitem["window_MSAA"].IsInt())
				window_MSAA = (uint32_t)configitem["window_MSAA"].GetInt();

			string window_name = "";
			if (configitem.HasMember("window_name") && configitem["window_name"].IsString())
				window_name = configitem["window_name"].GetString();

			Vector2T<float> window_sizeload = {};
			if (configitem.HasMember("window_size") && configitem["window_size"].IsArray()) {

				window_sizeload.vector_x = (float)configitem["window_size"][0].GetInt();
				window_sizeload.vector_y = (float)configitem["window_size"][1].GetInt();
			}

			return_initflag |= !GLFWwindowInit(fw_version, window_MSAA, false, true);
			return_initflag |= !GLFWwindowCreate(CoreModuleWindow::WindowConfig((uint32_t)window_sizeload.vector_x, (uint32_t)window_sizeload.vector_y, window_name));
			return_initflag |= !GLFWwindowVsync(true);
			return_initflag |= !GLFWwindowCallback();

			// glfw window icon.
			if (configitem.HasMember("window_icon_file") && configitem["window_icon_file"].IsString())
				GLFWwindowSetIncon(configitem["window_icon_file"].GetString());

			// init 'COREDATAPACK' & 'COREDATAINFO'.
			COREDATAPACK.JsonFileLoader = ReadJsonConfig;
			COREDATAINFO.WindowSize = window_sizeload;

			// print device info.
			FSloadInformation* devinfo = new FSloadInformation[4]{
				"sysinfo platform gpu",
				"sysinfo memory capacity",
				"sysinfo memory size",
				"sysinfo opengl attribs"
			};
			delete[] devinfo;

			FSloadInformation texlimit("sysinfo opengl texture");
		    MaxNumberTMU = (uint32_t)texlimit.get_sizevalue();

			PushLogger(LOG_INFO, SYSMD_LB_CORE, "imgui gui config...");

			string shader_version = {}, fonts_path = {};
			if (configitem.HasMember("window_sh_version") && configitem["window_sh_version"].IsString())
				shader_version = configitem["window_sh_version"].GetString();

			if (configitem.HasMember("imgui_fonts_file") && configitem["imgui_fonts_file"].IsString())
				fonts_path = configitem["imgui_fonts_file"].GetString();

			Vector4T<float> gui_text_color = {};
			if (configitem.HasMember("imgui_fonts_color") && configitem["imgui_fonts_color"].IsArray()) {

				gui_text_color.vector_x = configitem["imgui_fonts_color"][0].GetFloat();
				gui_text_color.vector_y = configitem["imgui_fonts_color"][1].GetFloat();
				gui_text_color.vector_z = configitem["imgui_fonts_color"][2].GetFloat();
				gui_text_color.vector_w = configitem["imgui_fonts_color"][3].GetFloat();
			}

			float fonts_size = 16.0f, frame_rounding = 0.0f, window_rounding = 0.0f;
			if (configitem.HasMember("imgui_fonts_size") && configitem["imgui_fonts_size"].IsFloat())
				fonts_size = configitem["imgui_fonts_size"].GetFloat();

			if (configitem.HasMember("imgui_window_rounding") && configitem["imgui_window_rounding"].IsFloat())
				window_rounding = configitem["imgui_window_rounding"].GetFloat();

			if (configitem.HasMember("imgui_frame_rounding") && configitem["imgui_frame_rounding"].IsFloat())
				frame_rounding = configitem["imgui_frame_rounding"].GetFloat();

			ImGuiInit(main_window_object,
				CoreModuleImGui::GuiConfig(
					shader_version,
					fonts_path,
					gui_text_color,
					fonts_size,
					frame_rounding,
					window_rounding
				)
			);

			// load shader config file => load shader.
			if (configitem.HasMember("shader_reg_file") && configitem["shader_reg_file"].IsString()) {
				ShaderConfigLoad(
					ReadJsonConfig(
						configitem["shader_reg_file"].GetString(),
						temp_flag[1]
					)
				);
			}
			return_initflag |= !temp_flag[1];

			// create thread pool.
			if (configitem.HasMember("system_create_workers") && configitem["system_create_workers"].IsUint())
				COREDATAPACK.TaskThreadPool = new ImThreadTask::ThreadPool(configitem["system_create_workers"].GetUint());

			// init user_object logic.
			for (auto it = COREDATAUSEROBJ.begin(); it != COREDATAUSEROBJ.end(); ++it)
				return_initflag |= !it->second->LogicInitialization(COREDATAPACK);
		}
		else
			PushLogger(LOG_ERR, SYSMD_LB_CORE, "system config object.");

		return !return_initflag;
	}

	void FrameworkImCore::ResourcesPackageFree(FRMCORE_PACKAGE& package) {
		// -------------------------------- [DATA] --------------------------------
		// free shader resources.
		for (auto it = package.DataShaderFx.begin(); it != package.DataShaderFx.end(); ++it) {

			SFglobalDeletel.delete_program_handle(it->second.ShaderProgramHandle);
			SFglobalDeletel.delete_model_handle(it->second.ModelHandle);
			SFglobalDeletel.delete_texture_handle(it->second.FrameBufferTex);
			SFglobalDeletel.delete_framebuffer_handle(it->second.FrameBufferHandle);

			for (auto& deltex : it->second.ParamTexture) {

				ShaderGlobalRes::ResTMU.TmuCountFreeTex2D(deltex);
				SFglobalDeletel.delete_texture_handle(deltex);
			}
		}
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
		for (auto it = package.StaticFlag.begin(); it != package.StaticFlag.end(); ++it)
			ImProFxTools::FrmStatic.StaticFlagDelete(package.StaticFlag, it->first.c_str());

		PushLogger(LOG_INFO, SYSMD_LB_CORE, "free resources flag_object.");

		// free timer resources.
		for (auto it = package.StaticTimer.begin(); it != package.StaticTimer.end(); ++it)
			ImProFxTools::FrmStatic.StaticTimerDelete(package.StaticTimer, it->first.c_str());

		PushLogger(LOG_INFO, SYSMD_LB_CORE, "free resources timer_object.");

		// free particle_system resources.
		for (auto it = package.ParticleSystem.begin(); it != package.ParticleSystem.end(); ++it)
			ImProFxTools::FrmStatic.StaticParticleSysDelete(package.ParticleSystem, it->first.c_str());

		PushLogger(LOG_INFO, SYSMD_LB_CORE, "free resources particle_system_object.");

		Vector2T<size_t> temp_param = {};
		if (ImProFxTools::FrmStatic.GetOperInfo(temp_param))
			PushLogger(LOG_INFO, SYSMD_LB_CORE, "trace operations 'static' not_error.");
		else
			PushLogger(LOG_WARN, SYSMD_LB_CORE, "trace operations 'static' alloc: %u delete: %u", temp_param.vector_x, temp_param.vector_y);

		// delete thread_pool object.
		delete package.TaskThreadPool;
	}

	bool FrameworkImCore::ProfxFree() {
		bool return_flag = false;

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
				string tmp_key = it->first;
				it->second->LogicCloseFree();

				// 防止一些吊毛在"LogicCloseFree()"里用反射删对象 /滑稽.
				if (COREDATAUSEROBJ.find(tmp_key) != COREDATAUSEROBJ.end()) {
					const string* tmp_name = &it->first;
					delete it->second;
					PushLogger(LOG_TRC, SYSMD_LB_CORE, "free user reg_obejct: %s", tmp_name->c_str());
				}
				else
					PushLogger(LOG_WARN, SYSMD_LB_CORE, "failed: free user reg_obejct 'LogicCloseFree' => nullptr: %s", it->first.c_str());
			}
			else
				PushLogger(LOG_ERR, SYSMD_LB_CORE, "failed: free user reg_obejct = nullptr: %s", it->first.c_str());

		ImGuiFree();
		return_flag |= !GLFWwindowFree();

		return !return_flag;
	}
}