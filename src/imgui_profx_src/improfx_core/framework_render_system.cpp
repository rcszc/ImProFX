// framework_render_system.
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "framework_render.h"

using namespace std;
using namespace LOGCONS;

namespace CoreModuleRender {

	rapidjson::Document FrameworkRender::ReadJsonConfig(string file) {
		rapidjson::Document configdoc;

		ifstream read_jsonfile(file);
		if (read_jsonfile.is_open()) {

			string json_string((std::istreambuf_iterator<char>(read_jsonfile)), std::istreambuf_iterator<char>());
			read_jsonfile.close();

			configdoc.Parse(json_string.c_str());

			LogInfoPush(LOG_INFO, SYSTEM_MODULE_LABLE_render + "read json config file: " + file);
		}
		else {
			LogInfoPush(LOG_ERR, SYSTEM_MODULE_LABLE_render + "failed read json config.");
			exit(-1);
		}
		return configdoc;
	}

	// 加载着色器配置.
	size_t FrameworkRender::ShaderConfigLoad(rapidjson::Document config) {
		size_t return_items = NULL;

		if (!config.HasParseError() && config.IsObject()) {
			// 加载着色器包.
			if (config.HasMember("core_shaderfx_config") && config["core_shaderfx_config"].IsArray()) {
				for (const auto& configitem : config["core_shaderfx_config"].GetArray()) {

					FxRenderItem DataTemp = {};
					string KeyNameTemp = {};

					if (configitem.HasMember("shaderfx_package") && configitem["shaderfx_package"].IsString())
						KeyNameTemp = configitem["shaderfx_package"].GetString();
					LogInfoPush(LOG_TRC, SYSTEM_MODULE_LABLE_render + "shader config item_name[key]: " + KeyNameTemp);

					// 加载着色器 vs, fs.
					FSloadShader load_shader;

					if (configitem.HasMember("shader_file_vertex") && configitem["shader_file_vertex"].IsString())
						load_shader.load_shader_vs(configitem["shader_file_vertex"].GetString());

					if (configitem.HasMember("shader_file_fragment") && configitem["shader_file_fragment"].IsString())
						load_shader.load_shader_fs(configitem["shader_file_fragment"].GetString());

					DataTemp.ShaderProgramHandle = load_shader.link_shader_program();
					load_shader.delete_shader();

					// 创建 FBO 附着纹理. 离屏渲染 =FBO=> 纹理.
					FScreateTexture CreateTexture(FS_TEXTURE_RGBA, WindowSize);
					FSloadFrameBuffer CreateFBO;
					CreateFBO.bind_framebuffer(CreateTexture.get_texture_handle());

					DataTemp.FrameBufferTex = FStextureHandle(CreateTexture.get_texture_handle(), DataTemp.ShaderProgramHandle, "FBTX");
					DataTemp.FrameBufferHandle = CreateFBO.get_framebuffer_handle();

					CreateTexture.unbind_texture();

					// 着色器 MVP 矩阵(ortho).
					float matrixcube = 1.0f;
					if (configitem.HasMember("matrix_ortho_cube") && configitem["matrix_ortho_cube"].IsFloat())
						matrixcube = configitem["matrix_ortho_cube"].GetFloat();

					glm::mat4 matmvp_tmp = glm::ortho(-matrixcube, matrixcube, -matrixcube, matrixcube, -matrixcube, matrixcube);

					const float* glmmatptr = glm::value_ptr(matmvp_tmp); // get glm matrix ptr.
					memcpy_s(DataTemp.RenderMVPmatrix.Matrix, 16 * sizeof(float), glmmatptr, 16 * sizeof(float));

					// load model_data VAO,VBO.
					FSdynamic ModelDataTemp = {};

					ModelDataTemp.VertexData = ShaderTemplateRect;
					ModelDataTemp.ModelDataLength = ShaderTemplateRectLen;

					// VAO handle index_number = 0, model type = 1.
					SFglobalTransmitData.load_model_data(ModelDataTemp, DataTemp.ModelHandle, 0);

					// load texture array.
					if (configitem.HasMember("shader_uniform_texture") && configitem["shader_uniform_texture"].IsArray()) {
						for (const auto& cfgitem : configitem["shader_uniform_texture"].GetArray()) {

							FStextureHandle TexTemp = {};
							if (cfgitem.HasMember("image_file_path") && cfgitem["image_file_path"].IsString()) {

								FSloadTexture loadimage(cfgitem["image_file_path"].GetString());
								TexTemp = loadimage.get_texture_handle(DataTemp.ShaderProgramHandle);
							}

							if (cfgitem.HasMember("image_ufh_name") && cfgitem["image_ufh_name"].IsString())
								TexTemp.uniform_name = cfgitem["image_ufh_name"].GetString();

							DataTemp.ParamTexture.push_back(TexTemp);
						}
					}

					// load uniform float.
					if (configitem.HasMember("shader_uniform_param") && configitem["shader_uniform_param"].IsArray())
						for (const auto& cfgitem : configitem["shader_uniform_param"].GetArray())
							DataTemp.ParamValueFloat[cfgitem.GetString()] = 1.0f;

					DataTemp.ExecuteProgramFlag = true;
					// hash_map add item.
					DataFxRender[KeyNameTemp] = DataTemp;
				}
			}
			else
				LogInfoPush(LOG_WARN, SYSTEM_MODULE_LABLE_render + "unable find core_shaderfx_config.");

			// 加载源贴图(不经shader处理)
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
					DataRawTexture[TexKey] = TexValue;
				}
			}
			else
				LogInfoPush(LOG_WARN, SYSTEM_MODULE_LABLE_render + "unable find core_raw_texture_config.");
		}
		else
			LogInfoPush(LOG_ERR, SYSTEM_MODULE_LABLE_render + "shader config object.");
		return return_items;
	}

	// 系统框架初始化.
	bool FrameworkRender::ProfxInit() {
		bool return_flag = false;

		rapidjson::Document CFG = ReadJsonConfig(DEF_CORE_SYSTEM_CONFIG);
		if (!CFG.HasParseError() && CFG.IsObject()) {

			// window config.
			LogInfoPush(LOG_INFO, SYSTEM_MODULE_LABLE_render + "opengl window config...");

			uint32_t fw_version[2] = { 2,0 };
			if (CFG.HasMember("window_fw_version") && CFG["window_fw_version"].IsArray()) {

				fw_version[0] = (uint32_t)CFG["window_fw_version"][0].GetInt();
				fw_version[1] = (uint32_t)CFG["window_fw_version"][1].GetInt();
			}

			uint32_t window_MSAA = 1;
			if (CFG.HasMember("window_MSAA") && CFG["window_MSAA"].IsInt())
				window_MSAA = (uint32_t)CFG["window_MSAA"].GetInt();

			string window_name = "";
			if (CFG.HasMember("window_name") && CFG["window_name"].IsString())
				window_name = CFG["window_name"].GetString();

			if (CFG.HasMember("window_size") && CFG["window_size"].IsArray()) {

				WindowSize.vector_x = (float)CFG["window_size"][0].GetInt();
				WindowSize.vector_y = (float)CFG["window_size"][1].GetInt();
			}

			return_flag |= !GLFWwindowInit(fw_version, window_MSAA, false, true);
			return_flag |= !GLFWwindowCreate(CoreModuleWindow::WindowConfig((uint32_t)WindowSize.vector_x, (uint32_t)WindowSize.vector_y, window_name));
			return_flag |= !GLFWwindowVsync(true);
			return_flag |= !GLFWwindowCallback();

			// print device info.
			FSloadInformation* devinfo = new FSloadInformation[4]{
					"sysinfo GPU","sysinfo MEMcpiy",
					"sysinfo MEMsize","sysinfo Attribs"
			};
			delete[] devinfo;

			FSloadInformation texlimit("sysinfo Texture");
		    MaxTextureSample = (uint32_t)texlimit.get_sizevalue();

			// shader window
			LogInfoPush(LOG_INFO, SYSTEM_MODULE_LABLE_render + "imgui gui config...");

			string shader_version = {}, fonts_path = {};
			if (CFG.HasMember("window_sh_version") && CFG["window_sh_version"].IsString())
				shader_version = CFG["window_sh_version"].GetString();

			if (CFG.HasMember("imgui_fonts_file") && CFG["imgui_fonts_file"].IsString())
				fonts_path = CFG["imgui_fonts_file"].GetString();

			Vector4T<float> gui_text_color = {};
			if (CFG.HasMember("imgui_fonts_color") && CFG["imgui_fonts_color"].IsArray()) {

				gui_text_color.vector_x = CFG["imgui_fonts_color"][0].GetFloat();
				gui_text_color.vector_y = CFG["imgui_fonts_color"][1].GetFloat();
				gui_text_color.vector_z = CFG["imgui_fonts_color"][2].GetFloat();
				gui_text_color.vector_w = CFG["imgui_fonts_color"][3].GetFloat();
			}

			float fonts_size = 16.0f, frame_rounding = 0.0f, window_rounding = 0.0f;
			if (CFG.HasMember("imgui_fonts_size") && CFG["imgui_fonts_size"].IsFloat())
				fonts_size = CFG["imgui_fonts_size"].GetFloat();

			if (CFG.HasMember("imgui_window_rounding") && CFG["imgui_window_rounding"].IsFloat())
				window_rounding = CFG["imgui_window_rounding"].GetFloat();

			if (CFG.HasMember("imgui_frame_rounding") && CFG["imgui_frame_rounding"].IsFloat())
				frame_rounding = CFG["imgui_frame_rounding"].GetFloat();

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

			// window_init => imgui_init => logic_init.
			return_flag |= !LogicInitialization();

			if (CFG.HasMember("shader_reg_file") && CFG["shader_reg_file"].IsString()) {
				ShaderConfigLoad(
					ReadJsonConfig(
						CFG["shader_reg_file"].GetString()
					)
				);
			}
		}
		else
			LogInfoPush(LOG_ERR, SYSTEM_MODULE_LABLE_render + "system config object.");

		return !return_flag;
	}

	bool FrameworkRender::ProfxFree() {
		bool return_flag = false;
		LogicCloseFree();

		// free shader resources.
		for (auto it = DataFxRender.begin(); it != DataFxRender.end(); ++it) {

			SFglobalDeletel.delete_shader_program(it->second.ShaderProgramHandle);
			SFglobalDeletel.delete_model_object(it->second.ModelHandle);
			SFglobalDeletel.delete_texture_object(it->second.FrameBufferTex);
			SFglobalDeletel.delete_framebuffer_object(it->second.FrameBufferHandle);

			for (auto& deltex : it->second.ParamTexture)
				SFglobalDeletel.delete_texture_object(deltex);
		}

		// free raw texture resources.
		for (auto it = DataRawTexture.begin(); it != DataRawTexture.end(); ++it)
			SFglobalDeletel.delete_texture_object(it->second);
		LogInfoPush(LOG_INFO, SYSTEM_MODULE_LABLE_render + "free resources texture_handle.");

		// free animation resources.
		for (auto it = DataCompAnim.begin(); it != DataCompAnim.end(); ++it) {
			if (it->second != nullptr)
				delete it->second;
		}
		LogInfoPush(LOG_INFO, SYSTEM_MODULE_LABLE_render + "free resources animation_object.");

		ImGuiFree();
		return_flag |= !GLFWwindowFree();

		return !return_flag;
	}
}

namespace CoreModuleSystem {

	int32_t ProfxSystemFramework::StartProfx() {
		int32_t loopreturnflag = 1;
		int32_t mainreturnflag = NULL;

		// start log process. timer:ms (600000:10min)
		LOGFILE::StartLogFileProcess("imgui_profx_system/syslog/", 600000);

		if (render_object->ProfxInit()) {

			while (loopreturnflag == 1) {
				loopreturnflag = render_object->ProfxEventLoop();
			}
			if (loopreturnflag == -1)
				mainreturnflag = STAGE_EXIT_LOOP;

			if (!render_object->ProfxFree())
				mainreturnflag = STAGE_EXIT_FREE;

			delete render_object;
		}
		else
			mainreturnflag = STAGE_EXIT_INIT;

		LOGFILE::FreeLogFileProcess();
		return mainreturnflag;
	}
}