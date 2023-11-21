// framework_core_system.
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "framework_core.hpp"

using namespace std;
using namespace LOGCONS;

namespace CoreModuleSystem {
	CoreModuleIMFX::FrameworkImCore* framework_global_object = nullptr;

	int32_t ProfxSystemFramework::StartProfx() {
		int32_t loopreturnflag = 1;
		int32_t mainreturnflag = NULL;

		// start log process (默认框架系统文件夹).
		LOGFILE::StartLogFileProcess("imgui_profx_system/syslog/");

		if (framework_global_object->ProfxInit()) {

			while (loopreturnflag == 1) {
				loopreturnflag = framework_global_object->ProfxEventLoop();
			}
			if (loopreturnflag == -1)
				mainreturnflag = STAGE_EXIT_LOOP;

			if (!framework_global_object->ProfxFree())
				mainreturnflag = STAGE_EXIT_FREE;

			delete framework_global_object;
		}
		else
			mainreturnflag = STAGE_EXIT_INIT;

		LOGFILE::FreeLogFileProcess(); // free process thread.
		return mainreturnflag;
	}

	void ProfxReflection::ReflectionObjectCreate(const string& ClassName, const string& ObjectName) {
		auto& registry = ReflectionFactory::GetRegistry();

		auto it = registry.find(ClassName);
		if (it != registry.end())
			framework_global_object->UserRegistrationObject(ClassName, ObjectName, it->second());
		else
			LogInfoPush(LOG_ERR, "[reflection]: failed: class not found in the registry.");
	}

	void ProfxReflection::ReflectionObjectDelete(const string& UniqueObjectName) {
		framework_global_object->UserDeleteObject(UniqueObjectName);
	}
}

CoreModuleSystem::ProfxSystemFramework IMFX_SYSTEM_OBJ;
CoreModuleSystem::ProfxReflection      IMFXREF;

namespace CoreModuleIMFX {

	rapidjson::Document FrameworkImCore::ReadJsonConfig(string file) {
		rapidjson::Document configdoc;

		ifstream read_jsonfile(file);
		if (read_jsonfile.is_open()) {
			// read char file.
			string json_string((istreambuf_iterator<char>(read_jsonfile)), istreambuf_iterator<char>());
			read_jsonfile.close();

			configdoc.Parse(json_string.c_str());
			LogInfoPush(LOG_INFO, SYSMD_LB_RENDER + "read json config file: " + file);
		}
		else {
			LogInfoPush(LOG_ERR, SYSMD_LB_RENDER + "failed read json config.");
			exit(-32);
		}
		return configdoc;
	}
	
	// 加载着色器配置.
	size_t FrameworkImCore::ShaderConfigLoad(rapidjson::Document config) {
		size_t return_items = NULL;
		// TMU alloc & VAO create count.
		uint32_t tmu_count_number = NULL, create_vao_count = NULL;

		// set vertex attribute count_begin.
		SFglobalTransmitData.set_vaoatt_begin(0);

		if (!config.HasParseError() && config.IsObject()) {
			if (config.HasMember("core_shaderfx_config") && config["core_shaderfx_config"].IsArray()) {
				// item / items.
				for (const auto& configitem : config["core_shaderfx_config"].GetArray()) {
					
					FxRenderItem ItemDataTemp = {}; // shader(包).
					string KeyNameTemp = {};

					if (configitem.HasMember("shaderfx_package") && configitem["shaderfx_package"].IsString())
						KeyNameTemp = configitem["shaderfx_package"].GetString();
					LogInfoPush(LOG_TRC, SYSMD_LB_RENDER + "shader config item_name[key]: " + KeyNameTemp);

					// 加载着色器 vs, fs.
					FSloadShader load_shader;

					if (configitem.HasMember("shader_file_vertex") && configitem["shader_file_vertex"].IsString())
						load_shader.load_shader_vs(configitem["shader_file_vertex"].GetString());

					if (configitem.HasMember("shader_file_fragment") && configitem["shader_file_fragment"].IsString())
						load_shader.load_shader_fs(configitem["shader_file_fragment"].GetString());
					
					ItemDataTemp.ShaderProgramHandle = load_shader.link_shader_program();
					load_shader.delete_shader();

					// 获取渲染分辨率.
					Vector2T<float> Resolution = Vector2T<float>(1, 1);
					if (configitem.HasMember("render_max_resolution") && configitem["render_max_resolution"].IsArray()) {

						Resolution.vector_x = (float)configitem["render_max_resolution"][0].GetInt();
						Resolution.vector_y = (float)configitem["render_max_resolution"][1].GetInt();
					}
					if (Resolution.vector_x == -1 && Resolution.vector_y == -1)
						Resolution = COREDATAINFO.WindowSize;

					Resolution.vector_x = abs(Resolution.vector_x);
					Resolution.vector_y = abs(Resolution.vector_y);

					// 创建 FBO 附着纹理. 离屏渲染 =FBO=> 纹理.
					FScreateTexture CreateTexture(FS_TEXTURE_RGBA, Resolution);
					FSloadFrameBuffer CreateFBO;
					CreateFBO.bind_framebuffer(CreateTexture.get_texture_handle());

					ItemDataTemp.FrameBufferTex = FStextureHandle(CreateTexture.get_texture_handle(), ItemDataTemp.ShaderProgramHandle, NULL, "FBTX");
					ItemDataTemp.FrameBufferHandle = CreateFBO.get_framebuffer_handle();

					CreateTexture.unbind_texture();

					// 着色器 MVP 矩阵(ortho).
					float matrixcube = 1.0f;
					if (configitem.HasMember("matrix_ortho_cube") && configitem["matrix_ortho_cube"].IsFloat())
						matrixcube = configitem["matrix_ortho_cube"].GetFloat();

					glm::mat4 matmvp_tmp = glm::ortho(-matrixcube, matrixcube, -matrixcube, matrixcube, -matrixcube, matrixcube);

					const float* glmmatptr = glm::value_ptr(matmvp_tmp); // get glm matrix ptr.
					memcpy_s(ItemDataTemp.RenderMVPmatrix.matrix, 16 * sizeof(float), glmmatptr, 16 * sizeof(float));

					// 最大渲染帧率.
					int64_t rate_time = NULL;
					if (configitem.HasMember("render_max_fps") && configitem["render_max_fps"].IsInt64())
						rate_time = configitem["render_max_fps"].GetInt64();

					// 计时(单位:微秒): (now - RenderRateTimer) > RenderRateTime ? : render
					ItemDataTemp.RenderRateTime = 1000 / rate_time * 1000;

					// load model_data(rect).
					FSdynamic ModelDataTemp = {};

					ModelDataTemp.VertexData      = ShaderTemplateRect;
					ModelDataTemp.ModelDataLength = ShaderTemplateRectLen;

					// transmit model_data(vao只需要创建一次).
					SFglobalTransmitData.load_model_data(ModelDataTemp, ItemDataTemp.ModelHandle, !(bool)create_vao_count);
					++create_vao_count;

					// load texture array.
					if (configitem.HasMember("shader_uniform_texture") && configitem["shader_uniform_texture"].IsArray()) {
						for (const auto& cfgitem : configitem["shader_uniform_texture"].GetArray()) {

							FStextureHandle TexTemp = {};
							if (cfgitem.HasMember("image_file_path") && cfgitem["image_file_path"].IsString()) {

								FSloadTexture loadimage(cfgitem["image_file_path"].GetString());
								TexTemp = loadimage.get_texture_handle(ItemDataTemp.ShaderProgramHandle);
							}

							if (cfgitem.HasMember("image_ufh_name") && cfgitem["image_ufh_name"].IsString())
								TexTemp.uniform_name = cfgitem["image_ufh_name"].GetString();

							TexTemp.tmu_count = tmu_count_number; // 因为这里要使用"glActiveTexture" (没有贴图合并 /悲
							++tmu_count_number;
							// sample unit number limit.
							tmu_count_number = tmu_count_number >= MaxNumberTMU ? MaxNumberTMU - 1 : tmu_count_number;

							ItemDataTemp.ParamTexture.push_back(TexTemp);
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
				LogInfoPush(LOG_WARN, SYSMD_LB_RENDER + "unable find core_shaderfx_config.");

			// 加载源贴图(不使用纹理采样器)
			if (config.HasMember("core_raw_texture_config") && config["core_raw_texture_config"].IsArray()) {
				for (const auto& configitem : config["core_raw_texture_config"].GetArray()) {

					FStextureHandle TexValue = {};
					string TexKey = {};

					if (configitem.HasMember("image_file_path") && configitem["image_file_path"].IsString()) {

						FSloadTexture loadimage(configitem["image_file_path"].GetString());
						TexValue = loadimage.get_texture_handle(NULL);
					}

					if (configitem.HasMember("image_unique_name") && configitem["image_unique_name"].IsString())
						TexKey = configitem["image_unique_name"].GetString();

					// hash_map add item.
					COREDATAPACK.DataTextureRaw[TexKey] = TexValue;
				}
			}
			else
				LogInfoPush(LOG_WARN, SYSMD_LB_RENDER + "unable find core_raw_texture_config.");
		}
		else
			LogInfoPush(LOG_ERR, SYSMD_LB_RENDER + "shader config object.");
		return return_items;
	}

	// 系统框架初始化.
	bool FrameworkImCore::ProfxInit() {
		bool return_flag = false;
		
		rapidjson::Document configitem = ReadJsonConfig(DEF_CORE_SYSTEM_CONFIG);
		if (!configitem.HasParseError() && configitem.IsObject()) {

			// window config.
			LogInfoPush(LOG_INFO, SYSMD_LB_RENDER + "opengl window config...");

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

			return_flag |= !GLFWwindowInit(fw_version, window_MSAA, false, true);
			return_flag |= !GLFWwindowCreate(CoreModuleWindow::WindowConfig((uint32_t)window_sizeload.vector_x, (uint32_t)window_sizeload.vector_y, window_name));
			return_flag |= !GLFWwindowVsync(true);
			return_flag |= !GLFWwindowCallback();

			// set main window icon.
			if (configitem.HasMember("window_icon_file") && configitem["window_icon_file"].IsString())
				GLFWwindowSetIncon(configitem["window_icon_file"].GetString());

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

			LogInfoPush(LOG_INFO, SYSMD_LB_RENDER + "imgui gui config...");

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
						configitem["shader_reg_file"].GetString()
					)
				);
			}

			// init user_object logic.
			for (auto it = COREDATAUSEROBJ.begin(); it != COREDATAUSEROBJ.end(); ++it)
				return_flag |= !it->second->LogicInitialization(COREDATAPACK);
		}
		else
			LogInfoPush(LOG_ERR, SYSMD_LB_RENDER + "system config object.");

		return !return_flag;
	}

	void FrameworkImCore::ResourcesUnorderedMapFree() {
		// free shader resources.
		for (auto it = COREDATAPACK.DataShaderFx.begin(); it != COREDATAPACK.DataShaderFx.end(); ++it) {

			SFglobalDeletel.delete_shader_program(it->second.ShaderProgramHandle);
			SFglobalDeletel.delete_model_object(it->second.ModelHandle);
			SFglobalDeletel.delete_texture_object(it->second.FrameBufferTex);
			SFglobalDeletel.delete_framebuffer_object(it->second.FrameBufferHandle);

			for (auto& deltex : it->second.ParamTexture)
				SFglobalDeletel.delete_texture_object(deltex);
		}
		LogInfoPush(LOG_INFO, SYSMD_LB_RENDER + "free resources shader_package.");

		// free raw texture resources.
		for (auto it = COREDATAPACK.DataTextureRaw.begin(); it != COREDATAPACK.DataTextureRaw.end(); ++it)
			SFglobalDeletel.delete_texture_object(it->second);
		LogInfoPush(LOG_INFO, SYSMD_LB_RENDER + "free resources texture_handle.");

		// free animation resources.
		for (auto it = COREDATAPACK.DataAnimation.begin(); it != COREDATAPACK.DataAnimation.end(); ++it)
			if (it->second != nullptr)
				delete it->second;

		LogInfoPush(LOG_INFO, SYSMD_LB_RENDER + "free resources animation_object.");

		// free memory resources.
		for (auto it = COREDATAPACK.DataDynamicMem.begin(); it != COREDATAPACK.DataDynamicMem.end(); ++it)
			if (it->second != nullptr)
				delete it->second;

		LogInfoPush(LOG_INFO, SYSMD_LB_RENDER + "free resources memory_object.");

		// free statictimer resources.
		for (auto it = COREDATAPACK.StaticTimer.begin(); it != COREDATAPACK.StaticTimer.end(); ++it)
			if (it->second != nullptr)
				delete it->second;

		LogInfoPush(LOG_INFO, SYSMD_LB_RENDER + "free resources statictimer_object.");
	}

	bool FrameworkImCore::ProfxFree() {
		bool return_flag = false;

		// delete resources hash_map.
		ResourcesUnorderedMapFree();

		// delete user object(register).
		for (auto it = COREDATAUSEROBJ.begin(); it != COREDATAUSEROBJ.end(); ++it)
			if (it->second != nullptr) {
				// free logic.
				string tmpkey = it->first;
				it->second->LogicCloseFree();

				// 防止一些吊毛在"LogicCloseFree()"里用反射删对象 /滑稽.
				if (COREDATAUSEROBJ.find(tmpkey) != COREDATAUSEROBJ.end()) {
					string tmp_name = it->first;
					delete it->second;
					LogInfoPush(LOG_TRC, SYSMD_LB_RENDER + "free user reg_obejct: " + tmp_name);
				}
				else
					LogInfoPush(LOG_WARN, SYSMD_LB_RENDER + "failed: free user reg_obejct 'LogicCloseFree' => nullptr: " + it->first);
			}
			else
				LogInfoPush(LOG_ERR, SYSMD_LB_RENDER + "failed: free user reg_obejct = nullptr: " + it->first);

		ImGuiFree();
		return_flag |= !GLFWwindowFree();

		return !return_flag;
	}
}