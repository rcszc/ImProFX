// framework_core_tools.
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "framework_core.hpp"

using namespace std;
using namespace LOGCONS;

#define IMTOOL_VALUESUB(high, value) high - value < 0.0f ? 0.0f : high - value && high - value > 1.0f ? 1.0f : high - value
ImVec4 ImColorDepthScale(const ImVec4& color, float value, float alpha) {
	return ImVec4(IMTOOL_VALUESUB(color.x, value), IMTOOL_VALUESUB(color.y, value), IMTOOL_VALUESUB(color.z, value), IMTOOL_VALUESUB(color.w, alpha));
}

namespace ImProFxTools {
	using CoreModuleIMFX::ChronoTimer;

	FxRenderGroup FrameworkCoreManager::FindGetRenderItem(unordered_map<string, FxRenderGroup>& input, strkey itemkey) {
		return (input.find(itemkey) != input.end()) ? input[itemkey] : FxRenderGroup();
	}

	FxRenderGroup* FrameworkCoreManager::FindGetRenderMapping(unordered_map<string, FxRenderGroup>& input, strkey itemkey) {
		return (input.find(itemkey) != input.end()) ? &input[itemkey] : nullptr;
	}

	uint32_t FrameworkCoreManager::FindGetRenderTexture(unordered_map<string, FxRenderGroup>& input, strkey itemkey) {
		return LLRES_Textures->ResourceFind(((input.find(itemkey) != input.end()) ? input[itemkey] : FxRenderGroup()).FrameBufferTexItem).Texture;
	}

	void FrameworkCoreManager::FindSetRenderItemParam(
		unordered_map<string, FxRenderGroup>& input,
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

	void FrameworkCoreManager::FindSetRenderItemFlag(unordered_map<string, FxRenderGroup>& input, strkey itemkey, bool flag) {
		auto it = input.find(itemkey);
		if (it != input.end())
			it->second.ExecuteProgramFlag = flag;
	}

	ImfxTexture FrameworkCoreManager::FindGetRawImage(unordered_map<string, string>& input, strkey itemkey) {
		return LLRES_Textures->ResourceFind((input.find(itemkey) != input.end()) ? input[itemkey] : string()).Texture;
	}

	bool FrameworkCoreOper::FindLoadRenderItem(unordered_map<string, FxRenderGroup>& input, strkey itemkey, FxRenderGroup fxshader) {
		auto it = input.find(itemkey);
		if (it != input.end()) {
			PushLogger(LOG_WARNING, SYSMD_LB_CORE, "failed load fx_render duplicate_key: %s", itemkey);
			return false;
		}
		else {
			if (LLRES_Shaders->ResourceFind(fxshader.ShaderProgramItem) == OGLinvalidHandle) {
				// shader handle == OGLinvalidHandle.
				PushLogger(LOG_WARNING, SYSMD_LB_CORE, "failed load fx_render invalid_shader key: %s", itemkey);
				return false;
			}
			input[itemkey] = fxshader;

			PushLogger(LOG_TRACE, SYSMD_LB_CORE, "load fx_render key: %s", itemkey);
			return true;
		}
	}

	bool FrameworkCoreOper::FindDeleteRenderItem(unordered_map<string, FxRenderGroup>& input, strkey itemkey) {
		auto it = input.find(itemkey);
		if (it != input.end()) {

			for (auto Tex : it->second.ParamTextureItems)
				LLRES_Textures->ResourceDelete(Tex);

			LLRES_Textures->ResourceDelete(it->second.FrameBufferTexItem);
			LLRES_VertexBuffers->ResourceDelete(it->second.ModelItem);
			LLRES_FrameBuffers->ResourceDelete(it->second.FrameBufferItem);
			LLRES_Shaders->ResourceDelete(it->second.ShaderProgramItem);

			input.erase(it);

			PushLogger(LOG_TRACE, SYSMD_LB_CORE, "delete fx_render key: %s", itemkey);
			return true;
		}
		else {
			PushLogger(LOG_WARNING, SYSMD_LB_CORE, "failed delete fx_render, not found key.");
			return false;
		}
	}

	bool FrameworkCoreOper::FindLoadRawImageData(unordered_map<string, string>& input, strkey itemkey, const SrcImage& data) {
		auto it = input.find(itemkey);
		if (it != input.end()) {
			PushLogger(LOG_WARNING, SYSMD_LB_CORE, "failed create raw_image(data) duplicate_key: %s", itemkey);
			return false;
		}
		else {
			IMFX_OGL_MAG::ImFXOGLtexture TextureProcess;
			IMPROFX_GEN_NUMBER GenResourceID;

			string StorageName = to_string(GenResourceID.IPFGEN_GenNumberTime());

			if (TextureProcess.CreateTexture(data.ImageWidth, data.ImageHeight, data.ImageChannels, data.ImagePixelsPointer, LinearFiltering)) {
				// raw_image TMU.count = 0.
				TextureProcess.TextureSamplerUnit = NULL;
				LLRES_Textures->ResourceStorage(StorageName, &TextureProcess);
			}
			input[itemkey] = StorageName;

			PushLogger(LOG_TRACE, SYSMD_LB_CORE, "create raw_image(data) key: %s", itemkey);
			return true;
		}
	}

	bool FrameworkCoreOper::FindLoadRawImageFile(unordered_map<string, string>& input, strkey itemkey, const string& file) {
		auto it = input.find(itemkey);
		if (it != input.end()) {
			PushLogger(LOG_WARNING, SYSMD_LB_CORE, "failed create raw_image(file) duplicate_key: %s", itemkey);
			return false;
		}
		else {
			IMFX_OGL_MAG::ImFXOGLtexture TextureProcess;
			IMPROFX_GEN_NUMBER GenResourceID;

			string StorageName = to_string(GenResourceID.IPFGEN_GenNumberTime());

			if (TextureProcess.CreateTextureLoader(file, LinearFiltering)) {
				// raw_image TMU.count = 0.
				TextureProcess.TextureSamplerUnit = NULL;
				LLRES_Textures->ResourceStorage(StorageName, &TextureProcess);
			}
			input[itemkey] = StorageName;
			
			PushLogger(LOG_TRACE, SYSMD_LB_CORE, "create raw_image(file) key: %s", itemkey);
			return true;
		}
	}

	bool FrameworkCoreOper::FindDeleteRawImage(std::unordered_map<std::string, string>& input, strkey itemkey) {
		auto it = input.find(itemkey);
		if (it != input.end()) {
			LLRES_Textures->ResourceDelete(it->second);
			input.erase(it);
			
			PushLogger(LOG_TRACE, SYSMD_LB_CORE, "delete raw_image key: %s", itemkey);
			return true;
		}
		else {
			PushLogger(LOG_WARNING, SYSMD_LB_CORE, "failed delete raw_image, not found key.");
			return false;
		}
	}

	bool FrameworkModule::FlagModuleCreate(unordered_map<string, bool>& input, strkey itemkey, bool init, bool duplicate) {
		auto it = input.find(itemkey);
		if (it != input.end() && duplicate) {
			PushLogger(LOG_WARNING, SYSMD_LB_CORE, "failed create flag_module, duplicate_key: %s", itemkey);
			return false;
		}
		else {
			input[itemkey] = init;
			++ModuleOperTrace.TraceOperationAllocating;

			PushLogger(LOG_TRACE, SYSMD_LB_CORE, "create flag_module key: %s", itemkey);
			return true;
		}
	}

	bool FrameworkModule::FlagModuleDelete(unordered_map<string, bool>& input, strkey itemkey, deletemode mode) {
		auto it = input.find(itemkey);
		if (it != input.end() && mode != STCMODE_DELETE_ALL) {
			input.erase(it);
			++ModuleOperTrace.TraceOperationDelete;

			PushLogger(LOG_TRACE, SYSMD_LB_CORE, "delete flag_module key: %s", itemkey);
			return true;
		}
		else if (mode == STCMODE_DELETE_ALL) {
			input.clear();
			PushLogger(LOG_TRACE, SYSMD_LB_CORE, "delete flag_module all.");
			return true;
		}
		else {
			PushLogger(LOG_WARNING, SYSMD_LB_CORE, "failed delete flag_module, not found key.");
			return false;
		}
	}

	bool* FrameworkModule::FlagModuleFind(unordered_map<string, bool>& input, strkey itemkey) {
		return (input.find(itemkey) != input.end()) ? &(input[itemkey]) : nullptr;
	}

	bool FrameworkModule::TimerModuleCreate(unordered_map<string, ChronoTimer*>& input, strkey itemkey) {
		auto it = input.find(itemkey);
		if (it != input.end()) {
			PushLogger(LOG_WARNING, SYSMD_LB_CORE, "failed create timer_module, duplicate_key: %s", itemkey);
			return false;
		}
		else {
			ChronoTimer* timercomp = new ChronoTimer();
			input[itemkey] = timercomp;
			++ModuleOperTrace.TraceOperationAllocating;

			PushLogger(LOG_TRACE, SYSMD_LB_CORE, "create timer_module key: %s", itemkey);
			return true;
		}
	}

	bool FrameworkModule::TimerModuleDelete(unordered_map<string, ChronoTimer*>& input, strkey itemkey, deletemode mode) {
		auto it = input.find(itemkey);
		if (it != input.end() && mode != STCMODE_DELETE_ALL) {
			if (it->second) {
				delete it->second;
				++ModuleOperTrace.TraceOperationDelete;
			}
			if (mode == STCMODE_DELETE)
				input.erase(it);

			PushLogger(LOG_TRACE, SYSMD_LB_CORE, "delete timer_module key: %s", itemkey);
			return true;
		}
		else if (mode == STCMODE_DELETE_ALL) {
			for (auto item : input) {
				if (item.second) {
					delete item.second;
					++ModuleOperTrace.TraceOperationDelete;
				}
			}
			input.clear();
			PushLogger(LOG_TRACE, SYSMD_LB_CORE, "delete timer_module all.");
			return true;
		}
		else {
			PushLogger(LOG_WARNING, SYSMD_LB_CORE, "failed delete timer_module, not found key.");
			return false;
		}
	}

	ChronoTimer* FrameworkModule::TimerModuleFind(unordered_map<string, ChronoTimer*>& input, strkey itemkey) {
		return (input.find(itemkey) != input.end()) ? (input[itemkey]) : nullptr;
	}

	bool FrameworkModule::ParticleModuleCreate(unordered_map<string, IFC_PARTC::ParticleBase*>& input, strkey itemkey, const Vector2T<float>& windowsize) {
		auto it = input.find(itemkey);
		if (it != input.end()) {
			PushLogger(LOG_WARNING, SYSMD_LB_CORE, "failed create particle_module, duplicate_key: %s", itemkey);
			return false;
		}
		else {
			ImParticle::ImParticleFx* create_ptcsys_object = new ImParticle::ImParticleFx(windowsize);
			input[itemkey] = create_ptcsys_object;
			++ModuleOperTrace.TraceOperationAllocating;

			PushLogger(LOG_TRACE, SYSMD_LB_CORE, "create particle_module key: %s", itemkey);
			return true;
		}
	}

	bool FrameworkModule::ParticleModuleDelete(unordered_map<string, IFC_PARTC::ParticleBase*>& input, strkey itemkey, deletemode mode) {
		auto it = input.find(itemkey);
		if (it != input.end() && mode != STCMODE_DELETE_ALL) {
			if (it->second) {
				delete it->second;
				++ModuleOperTrace.TraceOperationDelete;
			}
			if (mode == STCMODE_DELETE)
				input.erase(it);

			PushLogger(LOG_TRACE, SYSMD_LB_CORE, "delete particle_module key: %s", itemkey);
			return true;
		}
		else if (mode == STCMODE_DELETE_ALL) {
			for (auto item : input) {
				if (item.second) {
					delete item.second;
					++ModuleOperTrace.TraceOperationDelete;
				}
			}
			input.clear();
			PushLogger(LOG_TRACE, SYSMD_LB_CORE, "delete particle_module all.");
			return true;
		}
		else {
			PushLogger(LOG_WARNING, SYSMD_LB_CORE, "failed delete particle_module, not found key.");
			return false;
		}
	}

	bool FrameworkModule::GetOperInfo(Vector2T<size_t>& number) {
		number.vector_x = ModuleOperTrace.TraceOperationAllocating;
		number.vector_y = ModuleOperTrace.TraceOperationDelete;
		return ModuleOperTrace.ComparingOper();
	}

	IFC_PARTC::ParticleBase* FrameworkModule::ParticleModuleFind(unordered_map<string, IFC_PARTC::ParticleBase*>& input, strkey itemkey) {
		return (input.find(itemkey) != input.end()) ? input[itemkey] : nullptr;
	}

	bool FrameworkVariable::VariableAnyCreate(unordered_map<string, any>& input, strkey itemkey, any init) {
		auto it = input.find(itemkey);
		if (it != input.end()) {
			PushLogger(LOG_WARNING, SYSMD_LB_CORE, "failed create variable, duplicate_key: %s", itemkey);
			return false;
		}
		else {
			input[itemkey] = init;
			PushLogger(LOG_INFO, SYSMD_LB_CORE, "create variable key: %s", itemkey);
			return true;
		}
	}

	bool FrameworkVariable::VariableAnyDelete(unordered_map<string, any>& input, strkey itemkey) {
		auto it = input.find(itemkey);
		if (it != input.end()) {

			string VariableTypeName = {};
			if (it->second.has_value())
				VariableTypeName = it->second.type().name();
			input.erase(it);

			PushLogger(LOG_INFO, SYSMD_LB_CORE, "delete variable key: %s, type: %s", itemkey, VariableTypeName.c_str());
			return true;
		}
		else {
			PushLogger(LOG_WARNING, SYSMD_LB_CORE, "failed delete variable, not found key.");
			return false;
		}
	}

	bool FrameworkVariable::VariableAnyModify(unordered_map<string, any>& input, strkey itemkey, any value) {
		auto it = input.find(itemkey);
		if (it != input.end()) {

			if (it->second.type() == value.type()) {
				it->second = value;
				return true;
			}
			PushLogger(LOG_WARNING, SYSMD_LB_CORE, "modify variable key: %s, s: %s != m: %s", itemkey,
				it->second.type().name(), value.type().name());
			return false;
		}
		else {
			PushLogger(LOG_WARNING, SYSMD_LB_CORE, "failed modify variable, not found key.");
			return false;
		}
	}

	any FrameworkVariable::VariableAnyFindGet(unordered_map<string, any>& input, strkey itemkey) {
		return (input.find(itemkey) != input.end()) ? input[itemkey] : any();
	}

	FrameworkCoreManager    FrmCore;
	FrameworkCoreOper FrmCoreOper;
	FrameworkModule  FrmModule;
	FrameworkVariable    FrmVariable;
}

namespace CoreModuleIMFX {

	void FrameworkImCore::UserRegistrationObject(const char* class_name, const char* unique_name, INTERFACE_PROFX_USER* reg_class) {
		auto it = COREDATAUSEROBJ.find(unique_name);
		if (it != COREDATAUSEROBJ.end())
			PushLogger(LOG_WARNING, SYSMD_LB_CORE, "registration ref_object repeat_name.");
		else {
			if (reg_class) {
				COREDATAUSEROBJ[unique_name] = reg_class;
				// add info.
				COREDATAINFO.UserRegistrationInfo.push_back(RegisterObjectInfo(class_name, unique_name));
				PushLogger(LOG_TRACE, SYSMD_LB_CORE, "registration ref_object name: %s", unique_name);
			}
			else
				PushLogger(LOG_ERROR, SYSMD_LB_CORE, "registration ref_object nullptr.");
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
			PushLogger(LOG_TRACE, SYSMD_LB_CORE, "delete ref_object name: %s", unique_name);
		}
		else
			PushLogger(LOG_ERROR, SYSMD_LB_CORE, "delete ref_object nullptr.");
	}

	INTERFACE_PROFX_USER* FrameworkImCore::UserFindRegObject(const char* unique_name) {
		return (COREDATAUSEROBJ.find(unique_name) != COREDATAUSEROBJ.end()) ? (COREDATAUSEROBJ[unique_name]) : nullptr;
	}

	bool ChronoTimer::PeriodicTimerFlag(float itl_time, float* loss_time) {
		bool return_flag = false;
		float result_timeloss = 0.0f;

		int64_t tmptime = chrono::duration_cast<chrono::microseconds>(chrono::steady_clock::now() - PeriodicTimer).count();
		if (tmptime >= int64_t(itl_time * 1000.0f)) {
			// update timer.
			PeriodicTimer = chrono::steady_clock::now();
			return_flag = true;
			result_timeloss = (float)tmptime - itl_time * 1000.0f;
		}
		if (loss_time)
			*loss_time = result_timeloss;

		return return_flag;
	}

	float ChronoTimer::ContextTimerUse(function<void()> context_function) {
		float return_time = 0.0f;

		ContextTimer = chrono::steady_clock::now();
		context_function();
		return float(chrono::duration_cast<chrono::microseconds>(chrono::steady_clock::now() - ContextTimer).count()) / 1000.0f;
	}
}