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

	uint32_t FrameworkDataFind::FindRawTextureImg(unordered_map<string, FsTextureHandle>& input, strkey itemkey) {
		return ((input.find(itemkey) != input.end()) ? input[itemkey] : FsTextureHandle()).texture_hd;
	}

	bool FrameworkDataFind::FindLoadRawTextureImgData(unordered_map<string, FsTextureHandle>& input, strkey itemkey, const SrcImage& data) {
		auto it = input.find(itemkey);
		if (it != input.end()) {
			PushLogger(LOG_WARN, SYSMD_LB_CORE, "failed create raw_texture(data) duplicate_key: %s", itemkey);
			return false;
		}
		else {
			FSloadTexture loadsrc_texture;
			loadsrc_texture.load_source_data(data.imgpixel_ptr, Vector3T<uint32_t>(data.img_width, data.img_height, data.img_channels));
			input[itemkey] = loadsrc_texture.get_texture_handle(NULL, itemkey);

			PushLogger(LOG_TRC, SYSMD_LB_CORE, "create raw_texture(data) key: %s", itemkey);
			return true;
		}
	}

	bool FrameworkDataFind::FindLoadRawTextureImgFile(unordered_map<string, FsTextureHandle>& input, strkey itemkey, const string& file) {
		auto it = input.find(itemkey);
		if (it != input.end()) {
			PushLogger(LOG_WARN, SYSMD_LB_CORE, "failed create raw_texture(file) duplicate_key: %s", itemkey);
			return false;
		}
		else {
			FSloadTexture loadsrc_texture(file.c_str());
			input[itemkey] = loadsrc_texture.get_texture_handle(NULL, itemkey);

			PushLogger(LOG_TRC, SYSMD_LB_CORE, "create raw_texture(file) key: %s", itemkey);
			return true;
		}
	}

	bool FrameworkDataFind::FindDeleteRawTextureImg(std::unordered_map<std::string, FsTextureHandle>& input, strkey itemkey) {
		auto it = input.find(itemkey);
		if (it != input.end()) {
			SFglobalDeletel.delete_texture_handle(it->second);
			input.erase(it);
			PushLogger(LOG_TRC, SYSMD_LB_CORE, "delete raw_texture key: %s", itemkey);
			return true;
		}
		else {
			PushLogger(LOG_WARN, SYSMD_LB_CORE, "failed delete raw_texture, not found key.");
			return false;
		}
	}

	bool FrameworkStaticFind::StaticFlagAlloc(unordered_map<string, bool>& input, strkey itemkey, bool init, bool duplicate) {
		auto it = input.find(itemkey);
		if (it != input.end() && duplicate) {
			PushLogger(LOG_WARN, SYSMD_LB_CORE, "failed alloc static_flag, duplicate_key: %s", itemkey);
			return false;
		}
		else {
			input[itemkey] = init;
			++StaticTrace.TraceOperationAllocating;

			PushLogger(LOG_TRC, SYSMD_LB_CORE, "alloc static_flag key: %s", itemkey);
			return true;
		}
	}

	bool FrameworkStaticFind::StaticFlagDelete(unordered_map<string, bool>& input, strkey itemkey, const bool& erase) {
		auto it = input.find(itemkey);
		if (it != input.end() && erase) {
			input.erase(it);
			++StaticTrace.TraceOperationDelete;

			PushLogger(LOG_TRC, SYSMD_LB_CORE, "delete static_flag key: %s", itemkey);
			return true;
		}
		else {
			PushLogger(LOG_WARN, SYSMD_LB_CORE, "failed delete static_flag, not found key.");
			return false;
		}
	}

	bool* FrameworkStaticFind::StaticFlagFind(unordered_map<string, bool>& input, strkey itemkey) {
		return (input.find(itemkey) != input.end()) ? &(input[itemkey]) : nullptr;
	}

	bool FrameworkStaticFind::StaticTimerAlloc(unordered_map<string, ChronoTimer*>& input, strkey itemkey) {
		auto it = input.find(itemkey);
		if (it != input.end()) {
			PushLogger(LOG_WARN, SYSMD_LB_CORE, "failed alloc static_timer, duplicate_key: %s", itemkey);
			return false;
		}
		else {
			ChronoTimer* timercomp = new ChronoTimer();
			input[itemkey] = timercomp;
			++StaticTrace.TraceOperationAllocating;

			PushLogger(LOG_TRC, SYSMD_LB_CORE, "alloc static_timer key: %s", itemkey);
			return true;
		}
	}

	bool FrameworkStaticFind::StaticTimerDelete(unordered_map<string, ChronoTimer*>& input, strkey itemkey, const bool& erase) {
		auto it = input.find(itemkey);
		if (it != input.end()) {
			if (it->second) {
				delete it->second;
				++StaticTrace.TraceOperationDelete;
			}
			if (erase)
				input.erase(it);

			PushLogger(LOG_TRC, SYSMD_LB_CORE, "delete static_timer key: %s, erase: %d", itemkey, (int)erase);
			return true;
		}
		else {
			PushLogger(LOG_WARN, SYSMD_LB_CORE, "failed delete static_timer, not found key.");
			return false;
		}
	}

	ChronoTimer* FrameworkStaticFind::StaticTimerFind(unordered_map<string, ChronoTimer*>& input, strkey itemkey) {
		return (input.find(itemkey) != input.end()) ? (input[itemkey]) : nullptr;
	}

	bool FrameworkStaticFind::StaticParticleSysAlloc(unordered_map<string, IFC_PARTC::ParticleBase*>& input, strkey itemkey, const Vector2T<float>& windowsize) {
		auto it = input.find(itemkey);
		if (it != input.end()) {
			PushLogger(LOG_WARN, SYSMD_LB_CORE, "failed alloc particle_sys object, duplicate_key: %s", itemkey);
			return false;
		}
		else {
			ImParticle::ImParticleFx* create_ptcsys_object = new ImParticle::ImParticleFx(windowsize);
			input[itemkey] = create_ptcsys_object;
			++StaticTrace.TraceOperationAllocating;

			PushLogger(LOG_TRC, SYSMD_LB_CORE, "alloc particle_sys object key: %s", itemkey);
			return true;
		}
	}

	bool FrameworkStaticFind::StaticParticleSysDelete(unordered_map<string, IFC_PARTC::ParticleBase*>& input, strkey itemkey, const bool& erase) {
		auto it = input.find(itemkey);
		if (it != input.end()) {
			if (it->second) {
				delete it->second;
				++StaticTrace.TraceOperationDelete;
			}
			if (erase)
				input.erase(it);

			PushLogger(LOG_TRC, SYSMD_LB_CORE, "delete particle_sys object key: %s, erase: %d", itemkey, (int)erase);
			return true;
		}
		else {
			PushLogger(LOG_WARN, SYSMD_LB_CORE, "failed delete particle_sys object, not found key.");
			return false;
		}
	}

	bool FrameworkStaticFind::GetOperInfo(Vector2T<size_t>& number) {
		number.vector_x = StaticTrace.TraceOperationAllocating;
		number.vector_y = StaticTrace.TraceOperationDelete;
		return StaticTrace.ComparingOper();
	}

	IFC_PARTC::ParticleBase* FrameworkStaticFind::StaticParticleSysFind(unordered_map<string, IFC_PARTC::ParticleBase*>& input, strkey itemkey) {
		return (input.find(itemkey) != input.end()) ? input[itemkey] : nullptr;
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
		fragment_shader_script = SystemDefaultShaderFS;
		fragment_shader_script += text;
	}

	void FrameworkInline::ShaderProgramConfig(
		std::unordered_map<std::string, FxRenderItem>& input,
		const char*                                    key,
		const Vector2T<float>                          resolution,
		const int64_t                                  rate
	) {
		// vertex shader preset script.
		vertex_shader_script = SystemDefaultShaderVS;
		PushLogger(LOG_TRC, SYSMD_LB_CORE, "inline shader load & config... key: %s", key);

		FxRenderItem data_temp = {};
		FSloadShader compile_shader;

		compile_shader.load_shader_vs(vertex_shader_script.c_str(), IFC_SHADER::StringScript);
		compile_shader.load_shader_fs(fragment_shader_script.c_str(), IFC_SHADER::StringScript);

		data_temp.ShaderProgramHandle = compile_shader.create_program_handle();
		compile_shader.delete_shader();

		FScreateTexture create_texture(TextureColorRGBA, resolution);
		FSloadFrameBuffer create_fbo;
		create_fbo.create_bind_framebuffer(create_texture.get_texture_handle());

		data_temp.FrameBufferTex    = FsTextureHandle(create_texture.get_texture_handle(), data_temp.ShaderProgramHandle, NULL, "FBTX");
		data_temp.FrameBufferHandle = create_fbo.get_framebuffer_handle();

		create_texture.unbind_texture();

		glm::mat4 matmvp_tmp = glm::ortho(
			-matrix_ortho_cube, matrix_ortho_cube, -matrix_ortho_cube, 
			matrix_ortho_cube, -matrix_ortho_cube, matrix_ortho_cube
		);

		const float* glmmatptr = glm::value_ptr(matmvp_tmp); // get glm matrix ptr.
		memcpy_s(data_temp.RenderMatrixMvp.matrix, 16 * sizeof(float), glmmatptr, 16 * sizeof(float));

		data_temp.RenderRateTime = 1000 / rate * 1000;

		FsVertexGroups modeldata_temp = {};

		modeldata_temp.VertexData      = ShaderTemplateRect;
		modeldata_temp.VertexDataLen = ShaderTemplateRectLen;

		// transmit model_data.
		SFglobalTransmitData.load_vertex_static(
			ShaderGlobalRes::ResVAO.VaoHandleFind("FxCoreVao"),
			SFglobalTransmitData.create_vbo_handle(), 
			modeldata_temp, 
			data_temp.ModelHandle
		);

		data_temp.ExecuteProgramFlag = true;
		// add hash.map
		input[key] = data_temp;
		PushLogger(LOG_INFO, SYSMD_LB_CORE, "inline shader add_package.");
	}

	FrameworkDataFind   FrmData;
	FrameworkStaticFind FrmStatic;
	FrameworkTaskFind   FrmTask;
}

namespace CoreModuleIMFX {

	void FrameworkImCore::UserRegistrationObject(const char* class_name, const char* unique_name, INTERFACE_PROFX_USER* reg_class) {
		auto it = COREDATAUSEROBJ.find(unique_name);
		if (it != COREDATAUSEROBJ.end())
			PushLogger(LOG_WARN, SYSMD_LB_CORE, "registration user_object repeat_name.");
		else {
			if (reg_class) {
				COREDATAUSEROBJ[unique_name] = reg_class;
				// add info.
				COREDATAINFO.UserRegistrationInfo.push_back(regobjectinfo(class_name, unique_name));
				PushLogger(LOG_TRC, SYSMD_LB_CORE, "registration user_object name: %s", unique_name);
			}
			else
				PushLogger(LOG_ERR, SYSMD_LB_CORE, "registration user_object nullptr.");
		}
	}

	void FrameworkImCore::UserDeleteObject(const char* unique_name) {
		auto tmpobj = UserFindRegObject(unique_name);
		if (tmpobj) {
			// delete reg_info.
			for (auto it = COREDATAINFO.UserRegistrationInfo.begin(); it != COREDATAINFO.UserRegistrationInfo.end();)
				if (it->UniqueObjName == unique_name)
					it = COREDATAINFO.UserRegistrationInfo.erase(it);
			delete tmpobj;
			// delete map_item.
			COREDATAUSEROBJ.erase(unique_name);
			PushLogger(LOG_TRC, SYSMD_LB_CORE, "delete user_object name: %s", unique_name);
		}
		else
			PushLogger(LOG_ERR, SYSMD_LB_CORE, "delete user_object nullptr.");
	}

	INTERFACE_PROFX_USER* FrameworkImCore::UserFindRegObject(const char* unique_name) {
		return (COREDATAUSEROBJ.find(unique_name) != COREDATAUSEROBJ.end()) ? (COREDATAUSEROBJ[unique_name]) : nullptr;
	}

	bool ChronoTimer::PeriodicTimerFlag(float itl_time, float* loss_time) {
		bool return_flag = false;
		float resule_loss = 0.0f;

		int64_t tmptime = chrono::duration_cast<chrono::microseconds>(chrono::steady_clock::now() - periodic_timer).count();
		if (tmptime >= int64_t(itl_time * 1000.0f)) {
			// update timer.
			periodic_timer = chrono::steady_clock::now();
			return_flag = true;
			resule_loss = (float)tmptime - itl_time * 1000.0f;
		}
		if (loss_time)
			*loss_time = resule_loss;

		return return_flag;
	}

	float ChronoTimer::ContextTimerUse(function<void()> context_function) {
		float return_time = 0.0f;

		context_timer = chrono::steady_clock::now();
		context_function();
		return float(chrono::duration_cast<chrono::microseconds>(chrono::steady_clock::now() - context_timer).count()) / 1000.0f;
	}
}