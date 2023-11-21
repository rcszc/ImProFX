// framework_core_tools.
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "framework_core.hpp"

using namespace std;
using namespace LOGCONS;

const char* SystemDefaultShaderVS = R"(
#version 460 core
layout (location = 0) in vec3 vertex_pos;
layout (location = 1) in vec4 vertex_color;
layout (location = 2) in vec2 vertex_texture;

uniform float FxTime;
uniform mat4  FxMatMvp;

out vec4 FxColor; 
out vec2 FxCoord;

void main()
{
	gl_Position = FxMatMvp * vec4(vertex_pos, 1);
	FxColor     = vertex_color;
	FxCoord     = vertex_texture;
}
)";

const char* SystemDefaultShaderFS = R"(
#version 460 core
in vec4 FxColor;
in vec2 FxCoord;

uniform vec2  FxWinSize;
uniform float FxTime;

out vec4 FragColor;
)";

namespace ImProFxTools {
	using CoreModuleIMFX::ChronoTimer;

	FxRenderItem FrameworkDataFind::FindRenderItem(unordered_map<string, FxRenderItem>& input, strkey itemkey) {
		return (input.find(itemkey) != input.end()) ? input[itemkey] : FxRenderItem();
	}

	uint32_t FrameworkDataFind::FindRenderItemFBO(unordered_map<string, FxRenderItem>& input, strkey itemkey) {
		return ((input.find(itemkey) != input.end()) ? input[itemkey] : FxRenderItem()).FrameBufferTex.texture_hd;
	}

	void FrameworkDataFind::FindSetRenderItemParam(
		unordered_map<string, FxRenderItem>& input, 
		strkey                               itemkey,
		strkey                               uniform_name,
		float                                value
	) {
		auto it = input.find(itemkey);
		if (it != input.end()) {
			auto ituf = it->second.ParamValueFloat.find(uniform_name);
			if (ituf != it->second.ParamValueFloat.end())
				ituf->second = value;
		}
	}

	void FrameworkDataFind::FindSetRenderItemFlag(unordered_map<string, FxRenderItem>& input, strkey itemkey, bool flag) {
		auto it = input.find(itemkey);
		if (it != input.end())
			it->second.ExecuteProgramFlag = flag;
	}

	uint32_t FrameworkDataFind::FindRawTextureImg(unordered_map<string, FStextureHandle>& input, strkey itemkey) {
		return ((input.find(itemkey) != input.end()) ? input[itemkey] : FStextureHandle()).texture_hd;
	}

	void FrameworkDataFind::FindLoadRawTextureImgData(unordered_map<string, FStextureHandle>& input, strkey itemkey, const SrcImage& data) {
		FSloadTexture loadsrc_texture;
		loadsrc_texture.load_srcdata_texture(data.imgpixel_ptr, Vector3T<uint32_t>(data.img_width, data.img_height, data.img_channels));
		input[itemkey] = loadsrc_texture.get_texture_handle(NULL);
	}

	void FrameworkDataFind::FindLoadRawTextureImgFile(
		unordered_map<string, FStextureHandle>& input, 
		strkey                                  itemkey,
		const string&                           file
	) {
		FSloadTexture loadsrc_texture(file);
		input[itemkey] = loadsrc_texture.get_texture_handle(NULL);
	}

	void FrameworkStaticFind::StaticFlagAlloc(unordered_map<string, bool>& input, strkey itemkey, bool init, bool REPEAT) {
		auto it = input.find(itemkey);
		if (it != input.end() && REPEAT)
			LogInfoPush(LOG_WARN, SYSMD_LB_RENDER + "alloc static_flag repeat_key: " + itemkey);
		else {
			input[itemkey] = init;
			LogInfoPush(LOG_INFO, SYSMD_LB_RENDER + "alloc static_flag key: " + itemkey);
		}
	}

	void FrameworkStaticFind::StaticFlagDelete(unordered_map<string, bool>& input, strkey itemkey) {
		auto it = input.find(itemkey);
		if (it != input.end()) {
			input.erase(it);
			LogInfoPush(LOG_INFO, SYSMD_LB_RENDER + "delete static_flag key: " + itemkey);
		}
		else
			LogInfoPush(LOG_INFO, SYSMD_LB_RENDER + "delete static_flag not found key.");
	}

	bool* FrameworkStaticFind::StaticFlagFind(unordered_map<string, bool>& input, strkey itemkey) {
		return (input.find(itemkey) != input.end()) ? &(input[itemkey]) : nullptr;
	}

	void FrameworkStaticFind::StaticTimerAlloc(unordered_map<string, ChronoTimer*>& input, strkey itemkey) {
		auto it = input.find(itemkey);
		if (it != input.end())
			LogInfoPush(LOG_WARN, SYSMD_LB_RENDER + "alloc static_timer repeat_key: " + itemkey);
		else {
			ChronoTimer* timercomp = new ChronoTimer();
			input[itemkey] = timercomp;
			LogInfoPush(LOG_INFO, SYSMD_LB_RENDER + "alloc static_timer key: " + itemkey);
		}
	}

	void FrameworkStaticFind::StaticTimerDelete(unordered_map<string, ChronoTimer*>& input, strkey itemkey) {
		auto it = input.find(itemkey);
		if (it != input.end()) {
			// object.delete => map.erase
			delete it->second;
			input.erase(it);
			LogInfoPush(LOG_INFO, SYSMD_LB_RENDER + "delete static_timer key: " + itemkey);
		}
		else
			LogInfoPush(LOG_INFO, SYSMD_LB_RENDER + "delete static_timer not found key.");
	}

	ChronoTimer* FrameworkStaticFind::StaticTimerFind(unordered_map<string, ChronoTimer*>& input, strkey itemkey) {
		return (input.find(itemkey) != input.end()) ? (input[itemkey]) : nullptr;
	}

	void FrameworkTaskFind::TaskAsyncCreate(unordered_map<string, shared_future<void>>& input, strkey itemkey, future<void> async) {
		input[itemkey] = move(async);
	}

	bool FrameworkTaskFind::TaskAsyncPromise(unordered_map<string, shared_future<void>>& input, strkey itemkey, const bool& reserve) {
		bool return_flag = false;

		auto it = input.find(itemkey);
		if (it != input.end()) {
			if (it->second.wait_for(chrono::milliseconds(0)) == future_status::ready) {

				return_flag = true;
				if (!reserve)
					input.erase(it);
			}
		}
		return return_flag;
	}

	void FrameworkInline::ShaderFragmentInline(const string text) {
		// fragment shader preset script.
		FragmentShaderScript = SystemDefaultShaderFS;
		FragmentShaderScript += text;
	}

	void FrameworkInline::ShaderProgramConfig(
		std::unordered_map<std::string, FxRenderItem>& input,
		const char*                                    key,
		const Vector2T<float>                          resolution,
		const int64_t                                  rate
	) {
		// vertex shader preset script.
		VertexShaderScript = SystemDefaultShaderVS;
		LogInfoPush(LOG_TRC, SYSMD_LB_RENDER + "inline shader load & config... key: " + key);

		FxRenderItem data_temp = {};
		FSloadShader compile_shader;

		compile_shader.load_shader_vs(VertexShaderScript, StringScript);
		compile_shader.load_shader_fs(FragmentShaderScript, StringScript);

		data_temp.ShaderProgramHandle = compile_shader.link_shader_program();
		compile_shader.delete_shader();

		FScreateTexture create_texture(FS_TEXTURE_RGBA, resolution);
		FSloadFrameBuffer create_fbo;
		create_fbo.bind_framebuffer(create_texture.get_texture_handle());

		data_temp.FrameBufferTex    = FStextureHandle(create_texture.get_texture_handle(), data_temp.ShaderProgramHandle, NULL, "FBTX");
		data_temp.FrameBufferHandle = create_fbo.get_framebuffer_handle();

		create_texture.unbind_texture();

		glm::mat4 matmvp_tmp = glm::ortho(
			-matrix_ortho_cube, matrix_ortho_cube, -matrix_ortho_cube, 
			matrix_ortho_cube, -matrix_ortho_cube, matrix_ortho_cube
		);

		const float* glmmatptr = glm::value_ptr(matmvp_tmp); // get glm matrix ptr.
		memcpy_s(data_temp.RenderMVPmatrix.matrix, 16 * sizeof(float), glmmatptr, 16 * sizeof(float));

		data_temp.RenderRateTime = 1000 / rate * 1000;

		FSdynamic modeldata_temp = {};

		modeldata_temp.VertexData      = ShaderTemplateRect;
		modeldata_temp.ModelDataLength = ShaderTemplateRectLen;

		// transmit model_data.
		SFglobalTransmitData.load_model_data(modeldata_temp, data_temp.ModelHandle, false);

		data_temp.ExecuteProgramFlag = true;
		// add hash.map
		input[key] = data_temp;
		LogInfoPush(LOG_INFO, SYSMD_LB_RENDER + "inline shader add_package.");
	}

	FrameworkDataFind   FrmData;
	FrameworkStaticFind FrmStatic;
	FrameworkTaskFind   FrmTask;
}

namespace CoreModuleIMFX {

	void FrameworkImCore::UserRegistrationObject(const string& RegClassName, const string& RegUniqueName, INTERFACE_PROFX_USER* RegClass) {
		auto it = COREDATAUSEROBJ.find(RegUniqueName);
		if (it != COREDATAUSEROBJ.end())
			LogInfoPush(LOG_WARN, SYSMD_LB_RENDER + "registration user_object repeat_name.");
		else {
			if (RegClass) {
				COREDATAUSEROBJ[RegUniqueName] = RegClass;
				// add info.
				COREDATAINFO.USER_REGISTER_INFO_NAME.push_back(regobjectinfo(RegClassName, RegUniqueName));
				LogInfoPush(LOG_TRC, SYSMD_LB_RENDER + "registration user_object name: " + RegUniqueName);
			}
			else
				LogInfoPush(LOG_ERR, SYSMD_LB_RENDER + "registration user_object nullptr.");
		}
	}

	void FrameworkImCore::UserDeleteObject(const string& UniqueName) {
		auto tmpobj = UserFindRegObject(UniqueName);
		if (tmpobj) {
			// delete reg_info.
			for (auto it = COREDATAINFO.USER_REGISTER_INFO_NAME.begin(); it != COREDATAINFO.USER_REGISTER_INFO_NAME.end();)
				if (it->UniqueObjName == UniqueName)
					it = COREDATAINFO.USER_REGISTER_INFO_NAME.erase(it);
			delete tmpobj;
			// delete map_item.
			COREDATAUSEROBJ.erase(UniqueName);
			LogInfoPush(LOG_TRC, SYSMD_LB_RENDER + "delete user_object name: " + UniqueName);
		}
		else
			LogInfoPush(LOG_ERR, SYSMD_LB_RENDER + "delete user_object nullptr.");
	}

	INTERFACE_PROFX_USER* FrameworkImCore::UserFindRegObject(const string& RegUniqueName) {
		return (COREDATAUSEROBJ.find(RegUniqueName) != COREDATAUSEROBJ.end()) ? (COREDATAUSEROBJ[RegUniqueName]) : nullptr;
	}

	bool ChronoTimer::PeriodicTimerFlag(float itl_time, float* loss_time) {
		bool return_flag = false;
		float resule_loss = 0.0f;

		int64_t tmptime = chrono::duration_cast<chrono::microseconds>(chrono::steady_clock::now() - PeriodicTimer).count();
		if (tmptime >= int64_t(itl_time * 1000.0f)) {
			// update timer.
			PeriodicTimer = chrono::steady_clock::now();
			return_flag = true;
			resule_loss = (float)tmptime - itl_time * 1000.0f;
		}
		if (loss_time)
			*loss_time = resule_loss;

		return return_flag;
	}

	float ChronoTimer::ContextTimerUse(function<void()> context_function) {
		float return_time = 0.0f;

		ContextTimer = chrono::steady_clock::now();
		context_function();
		return float(chrono::duration_cast<chrono::microseconds>(chrono::steady_clock::now() - ContextTimer).count()) / 1000.0f;
	}
}