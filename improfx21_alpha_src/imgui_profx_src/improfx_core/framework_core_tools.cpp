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

namespace system_tools {

	inline void DeleteFxShaderItem(FxRenderItem& fx_package) {
		// free shader res: program & model(vbo) & fbo_texture & framebuffer.
		SFglobalDeletel.delete_program_handle(fx_package.ShaderProgramHandle);
		SFglobalDeletel.delete_model_handle(fx_package.ModelHandle);
		SFglobalDeletel.delete_texture_handle(fx_package.FrameBufferTex);
		SFglobalDeletel.delete_framebuffer_handle(fx_package.FrameBufferHandle);

		for (auto& texture : fx_package.ParamTexture) {

			ShaderGlobalRes::ResTMU.TmuCountFreeTex2D(texture);
			SFglobalDeletel.delete_texture_handle(texture);
		}
	}
}

namespace ImProFxTools {
	using CoreModuleIMFX::ChronoTimer;

	void FxShaderLoader::ShaderFragmentScript(const string frag_script) {
		// vertex shader preset script.
		ShaderVertString = SystemDefaultShaderVS;
		// fragment shader preset script.
		ShaderFragString = SystemDefaultShaderFS;
		ShaderFragString += frag_script;
	}

	uint32_t FxShaderLoader::FxShaderProgramCreateCompile() {
		// compile shaders by mode.
		FSloadShader CompileShaders;
		CompileShaders.load_shader_vs(ShaderVertString.c_str(), Mode);
		CompileShaders.load_shader_fs(ShaderFragString.c_str(), Mode);

		FxShaderPackage.ShaderProgramHandle = CompileShaders.create_program_handle();
		CompileShaders.delete_shader();
		return FxShaderPackage.ShaderProgramHandle;
	}

	void FxShaderLoader::FxShaderProgramConfig(
		const Vector2T<uint32_t>& vertexhandle,
		const Vector2T<float>& resolution,
		const float& rate,
		const Vector2T<float>& render_scale
	) {
		PushLogger(LOG_INFO, SYSMD_LB_CORE, "<shader_loader>: loading fx_shader...");

		FScreateTexture CreateTexture(TextureColorRGBA, resolution);
		FSloadFrameBuffer CreateFrameBuffer;
		CreateFrameBuffer.create_bind_framebuffer(CreateTexture.get_texture_handle());

		FxShaderPackage.FrameBufferTex =
			FsTextureHandle(
				CreateTexture.get_texture_handle(),
				FxShaderPackage.ShaderProgramHandle,
				NULL,
				"FrameBufferObject"
			);
		FxShaderPackage.FrameBufferHandle = CreateFrameBuffer.get_framebuffer_handle();

		CreateTexture.unbind_texture();

		float OrthoCubeSacle = render_scale.vector_x / render_scale.vector_y;
		float OrthoCubeLength = ShaderDefaultOrthoLen;

		glm::mat4 ProjectionMatrix = glm::ortho(
			-OrthoCubeLength * OrthoCubeSacle, OrthoCubeLength * OrthoCubeSacle,
			-OrthoCubeLength, OrthoCubeLength,
			-OrthoCubeLength, OrthoCubeLength
		);

		const float* glmmatptr = glm::value_ptr(ProjectionMatrix);
		memcpy_s(FxShaderPackage.RenderMatrixMvp.matrix, 16 * sizeof(float), glmmatptr, 16 * sizeof(float));

		FxShaderPackage.RenderRateTime = uint64_t(1000.0f / rate * 1000.0f);

		FsVertexGroups ModelVerticesTemp = FsVertexGroups(ShaderTemplateRectLen, ShaderTemplateRect);
		SFglobalTransmitData.load_vertex_static(vertexhandle.vector_y, vertexhandle.vector_x, ModelVerticesTemp, FxShaderPackage.ModelHandle);

		FxShaderPackage.ExecuteProgramFlag = true;
		PushLogger(LOG_INFO, SYSMD_LB_CORE, "<shader_loader>: fx_shader loading completed.");
	}

	void FxShaderLoader::FxShaderParamLoader(const vector<FsTextureHandle>& uniform_texture, const unordered_map<string, float>& uniform_float) {
		FxShaderPackage.ParamTexture = uniform_texture;
		FxShaderPackage.ParamValueFloat = uniform_float;
	}

	FxRenderItem FrameworkCoreFind::FindRenderItem(unordered_map<string, FxRenderItem>& input, strkey itemkey) {
		return (input.find(itemkey) != input.end()) ? input[itemkey] : FxRenderItem();
	}

	uint32_t FrameworkCoreFind::FindRenderItemFBO(unordered_map<string, FxRenderItem>& input, strkey itemkey) {
		return ((input.find(itemkey) != input.end()) ? input[itemkey] : FxRenderItem()).FrameBufferTex.TextureHandle;
	}

	void FrameworkCoreFind::FindSetRenderItemParam(
		unordered_map<string, FxRenderItem>& input,
		strkey                               itemkey,
		strkey                               UniformName,
		float                                value
	) {
		auto it = input.find(itemkey);
		if (it != input.end()) {
			auto ituf = it->second.ParamValueFloat.find(UniformName);
			if (ituf != it->second.ParamValueFloat.end())
				ituf->second = value;
		}
	}

	void FrameworkCoreFind::FindSetRenderItemFlag(unordered_map<string, FxRenderItem>& input, strkey itemkey, bool flag) {
		auto it = input.find(itemkey);
		if (it != input.end())
			it->second.ExecuteProgramFlag = flag;
	}

	uint32_t FrameworkCoreFind::FindRawTextureImg(unordered_map<string, FsTextureHandle>& input, strkey itemkey) {
		return ((input.find(itemkey) != input.end()) ? input[itemkey] : FsTextureHandle()).TextureHandle;
	}

	bool FrameworkCoreManager::FindLoadRenderItem(unordered_map<string, FxRenderItem>& input, strkey itemkey, FxShaderLoader& loader) {
		auto it = input.find(itemkey);
		if (it != input.end()) {
			PushLogger(LOG_WARN, SYSMD_LB_CORE, "failed create fx_shader duplicate_key: %s", itemkey);
			return false;
		}
		else {
			input[itemkey] = *loader.FxShader();
			PushLogger(LOG_TRC, SYSMD_LB_CORE, "create fx_shader key: %s", itemkey);
			return true;
		}
	}

	void FrameworkCoreManager::FindDeleteRenderItem(unordered_map<string, FxRenderItem>& input, strkey itemkey) {
		auto it = input.find(itemkey);
		if (it != input.end()) {
			system_tools::DeleteFxShaderItem(it->second);
			PushLogger(LOG_TRC, SYSMD_LB_CORE, "delete fx_shader key: %s", itemkey);
		}
		else
			PushLogger(LOG_WARN, SYSMD_LB_CORE, "failed delete fx_shader, not found key.");
	}

	bool FrameworkCoreManager::FindLoadRawTextureImgData(unordered_map<string, FsTextureHandle>& input, strkey itemkey, const SrcImage& data) {
		auto it = input.find(itemkey);
		if (it != input.end()) {
			PushLogger(LOG_WARN, SYSMD_LB_CORE, "failed create raw_texture(data) duplicate_key: %s", itemkey);
			return false;
		}
		else {
			FSloadTexture loadsrc_texture;
			loadsrc_texture.load_source_data(data.ImagePixelPointer, Vector3T<uint32_t>(data.ImageWidth, data.ImageHeight, data.ImageChannels));
			input[itemkey] = loadsrc_texture.get_texture_handle(NULL, itemkey);

			PushLogger(LOG_TRC, SYSMD_LB_CORE, "create raw_texture(data) key: %s", itemkey);
			return true;
		}
	}

	bool FrameworkCoreManager::FindLoadRawTextureImgFile(unordered_map<string, FsTextureHandle>& input, strkey itemkey, const string& file) {
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

	bool FrameworkCoreManager::FindDeleteRawTextureImg(std::unordered_map<std::string, FsTextureHandle>& input, strkey itemkey) {
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

	bool FrameworkStaticFind::StaticFlagDelete(unordered_map<string, bool>& input, strkey itemkey, deletemode mode) {
		auto it = input.find(itemkey);
		if (it != input.end() && mode != STCMODE_DELETE_ALL) {
			input.erase(it);
			++StaticTrace.TraceOperationDelete;

			PushLogger(LOG_TRC, SYSMD_LB_CORE, "delete static_flag key: %s", itemkey);
			return true;
		}
		else if (mode == STCMODE_DELETE_ALL) {
			input.clear();
			PushLogger(LOG_TRC, SYSMD_LB_CORE, "delete static_flag all.");
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

	bool FrameworkStaticFind::StaticTimerDelete(unordered_map<string, ChronoTimer*>& input, strkey itemkey, deletemode mode) {
		auto it = input.find(itemkey);
		if (it != input.end() && mode != STCMODE_DELETE_ALL) {
			if (it->second) {
				delete it->second;
				++StaticTrace.TraceOperationDelete;
			}
			if (mode == STCMODE_DELETE)
				input.erase(it);

			PushLogger(LOG_TRC, SYSMD_LB_CORE, "delete static_timer key: %s", itemkey);
			return true;
		}
		else if (mode == STCMODE_DELETE_ALL) {
			for (auto item : input) {
				if (item.second) {
					delete item.second;
					++StaticTrace.TraceOperationDelete;
				}
			}
			input.clear();
			PushLogger(LOG_TRC, SYSMD_LB_CORE, "delete static_timer all.");
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

	bool FrameworkStaticFind::StaticParticleSysDelete(unordered_map<string, IFC_PARTC::ParticleBase*>& input, strkey itemkey, deletemode mode) {
		auto it = input.find(itemkey);
		if (it != input.end() && mode != STCMODE_DELETE_ALL) {
			if (it->second) {
				delete it->second;
				++StaticTrace.TraceOperationDelete;
			}
			if (mode == STCMODE_DELETE)
				input.erase(it);

			PushLogger(LOG_TRC, SYSMD_LB_CORE, "delete particle_sys object key: %s", itemkey);
			return true;
		}
		else if (mode == STCMODE_DELETE_ALL) {
			for (auto item : input) {
				if (item.second) {
					delete item.second;
					++StaticTrace.TraceOperationDelete;
				}
			}
			input.clear();
			PushLogger(LOG_TRC, SYSMD_LB_CORE, "delete particle_sys all.");
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

	void FrameworkTaskFind::AsyncTaskCreate(unordered_map<string, shared_future<void>>& input, strkey itemkey, future<void> async) {
		input[itemkey] = move(async);
	}

	bool FrameworkTaskFind::AsyncTaskPromise(unordered_map<string, shared_future<void>>& input, strkey itemkey, const bool& reserve) {
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

	FrameworkCoreFind    FrmCore;
	FrameworkCoreManager FrmCoreOper;
	FrameworkStaticFind  FrmStatic;
	FrameworkTaskFind    FrmTask;
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
				COREDATAINFO.UserRegistrationInfo.push_back(RegisterObjectInfo(class_name, unique_name));
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
				if (it->UniqueObjectName == unique_name)
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