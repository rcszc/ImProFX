// framework_core_tools.
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "framework_core.hpp"

using namespace std;
using namespace LOGCONS;

namespace ImProFxTools {
	using CoreModuleIMFX::ChronoTimer;

	FxRenderItem FrameworkCoreFind::FindRenderItem(unordered_map<string, FxRenderItem>& input, strkey itemkey) {
		return (input.find(itemkey) != input.end()) ? input[itemkey] : FxRenderItem();
	}

	uint32_t FrameworkCoreFind::FindRenderItemFBO(unordered_map<string, FxRenderItem>& input, strkey itemkey) {
		return LLRES_Textures->ResourceFind(((input.find(itemkey) != input.end()) ? input[itemkey] : FxRenderItem()).FrameBufferTexItem).Texture;
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

	uint32_t FrameworkCoreFind::FindRawTextureImg(unordered_map<string, string>& input, strkey itemkey) {
		return LLRES_Textures->ResourceFind((input.find(itemkey) != input.end()) ? input[itemkey] : string()).Texture;
	}

	/*
	* OLD_VERSION 2.1 => 2.2 FxShaderLoad, FxShaderDelete
	* bool FrameworkCoreManager::FindLoadRenderItem(unordered_map<string, FxRenderItem>& input, strkey itemkey, FxShaderLoader& loader)
	* void FrameworkCoreManager::FindDeleteRenderItem(unordered_map<string, FxRenderItem>& input, strkey itemkey)
	*/

	bool FrameworkCoreManager::FindLoadRawTextureImgData(unordered_map<string, string>& input, strkey itemkey, const SrcImage& data) {
		auto it = input.find(itemkey);
		if (it != input.end()) {
			PushLogger(LOG_WARNING, SYSMD_LB_CORE, "failed create raw_texture(data) duplicate_key: %s", itemkey);
			return false;
		}
		else {
			IMFX_OGL_MAG::ImFXOGLtexture TextureProcess;
			IMPROFX_GEN_NUMBER GenResourceID;

			auto StorageName = string(itemkey) + LLRES_ADCHAR + to_string(GenResourceID.IPFGEN_GenNumberTime()) + LLRES_ADCHAR;

			if (TextureProcess.CreateTexture(data.ImageWidth, data.ImageHeight, data.ImageChannels, data.ImagePixelsPointer, LinearFiltering))
				LLRES_Textures->ResourceStorage(StorageName, &TextureProcess);

			input[itemkey] = StorageName;

			PushLogger(LOG_TRACE, SYSMD_LB_CORE, "create raw_texture(data) key: %s", itemkey);
			return true;
		}
	}

	bool FrameworkCoreManager::FindLoadRawTextureImgFile(unordered_map<string, string>& input, strkey itemkey, const string& file) {
		auto it = input.find(itemkey);
		if (it != input.end()) {
			PushLogger(LOG_WARNING, SYSMD_LB_CORE, "failed create raw_texture(file) duplicate_key: %s", itemkey);
			return false;
		}
		else {
			IMFX_OGL_MAG::ImFXOGLtexture TextureProcess;
			IMPROFX_GEN_NUMBER GenResourceID;

			auto StorageName = string(itemkey) + LLRES_ADCHAR + to_string(GenResourceID.IPFGEN_GenNumberTime()) + LLRES_ADCHAR;

			if (TextureProcess.CreateTextureLoader(file, LinearFiltering))
				LLRES_Textures->ResourceStorage(StorageName, &TextureProcess);

			input[itemkey] = StorageName;
			
			PushLogger(LOG_TRACE, SYSMD_LB_CORE, "create raw_texture(file) key: %s", itemkey);
			return true;
		}
	}

	bool FrameworkCoreManager::FindDeleteRawTextureImg(std::unordered_map<std::string, string>& input, strkey itemkey) {
		auto it = input.find(itemkey);
		if (it != input.end()) {
			LLRES_Textures->ResourceDelete(it->second);
			input.erase(it);
			
			PushLogger(LOG_TRACE, SYSMD_LB_CORE, "delete raw_texture key: %s", itemkey);
			return true;
		}
		else {
			PushLogger(LOG_WARNING, SYSMD_LB_CORE, "failed delete raw_texture, not found key.");
			return false;
		}
	}

	bool FrameworkStaticFind::StaticFlagAlloc(unordered_map<string, bool>& input, strkey itemkey, bool init, bool duplicate) {
		auto it = input.find(itemkey);
		if (it != input.end() && duplicate) {
			PushLogger(LOG_WARNING, SYSMD_LB_CORE, "failed alloc static_flag, duplicate_key: %s", itemkey);
			return false;
		}
		else {
			input[itemkey] = init;
			++StaticTrace.TraceOperationAllocating;

			PushLogger(LOG_TRACE, SYSMD_LB_CORE, "alloc static_flag key: %s", itemkey);
			return true;
		}
	}

	bool FrameworkStaticFind::StaticFlagDelete(unordered_map<string, bool>& input, strkey itemkey, deletemode mode) {
		auto it = input.find(itemkey);
		if (it != input.end() && mode != STCMODE_DELETE_ALL) {
			input.erase(it);
			++StaticTrace.TraceOperationDelete;

			PushLogger(LOG_TRACE, SYSMD_LB_CORE, "delete static_flag key: %s", itemkey);
			return true;
		}
		else if (mode == STCMODE_DELETE_ALL) {
			input.clear();
			PushLogger(LOG_TRACE, SYSMD_LB_CORE, "delete static_flag all.");
			return true;
		}
		else {
			PushLogger(LOG_WARNING, SYSMD_LB_CORE, "failed delete static_flag, not found key.");
			return false;
		}
	}

	bool* FrameworkStaticFind::StaticFlagFind(unordered_map<string, bool>& input, strkey itemkey) {
		return (input.find(itemkey) != input.end()) ? &(input[itemkey]) : nullptr;
	}

	bool FrameworkStaticFind::StaticTimerAlloc(unordered_map<string, ChronoTimer*>& input, strkey itemkey) {
		auto it = input.find(itemkey);
		if (it != input.end()) {
			PushLogger(LOG_WARNING, SYSMD_LB_CORE, "failed alloc static_timer, duplicate_key: %s", itemkey);
			return false;
		}
		else {
			ChronoTimer* timercomp = new ChronoTimer();
			input[itemkey] = timercomp;
			++StaticTrace.TraceOperationAllocating;

			PushLogger(LOG_TRACE, SYSMD_LB_CORE, "alloc static_timer key: %s", itemkey);
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

			PushLogger(LOG_TRACE, SYSMD_LB_CORE, "delete static_timer key: %s", itemkey);
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
			PushLogger(LOG_TRACE, SYSMD_LB_CORE, "delete static_timer all.");
			return true;
		}
		else {
			PushLogger(LOG_WARNING, SYSMD_LB_CORE, "failed delete static_timer, not found key.");
			return false;
		}
	}

	ChronoTimer* FrameworkStaticFind::StaticTimerFind(unordered_map<string, ChronoTimer*>& input, strkey itemkey) {
		return (input.find(itemkey) != input.end()) ? (input[itemkey]) : nullptr;
	}

	bool FrameworkStaticFind::StaticParticleSysAlloc(unordered_map<string, IFC_PARTC::ParticleBase*>& input, strkey itemkey, const Vector2T<float>& windowsize) {
		auto it = input.find(itemkey);
		if (it != input.end()) {
			PushLogger(LOG_WARNING, SYSMD_LB_CORE, "failed alloc particle_sys object, duplicate_key: %s", itemkey);
			return false;
		}
		else {
			ImParticle::ImParticleFx* create_ptcsys_object = new ImParticle::ImParticleFx(windowsize);
			input[itemkey] = create_ptcsys_object;
			++StaticTrace.TraceOperationAllocating;

			PushLogger(LOG_TRACE, SYSMD_LB_CORE, "alloc particle_sys object key: %s", itemkey);
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

			PushLogger(LOG_TRACE, SYSMD_LB_CORE, "delete particle_sys object key: %s", itemkey);
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
			PushLogger(LOG_TRACE, SYSMD_LB_CORE, "delete particle_sys all.");
			return true;
		}
		else {
			PushLogger(LOG_WARNING, SYSMD_LB_CORE, "failed delete particle_sys object, not found key.");
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
			PushLogger(LOG_WARNING, SYSMD_LB_CORE, "registration user_object repeat_name.");
		else {
			if (reg_class) {
				COREDATAUSEROBJ[unique_name] = reg_class;
				// add info.
				COREDATAINFO.UserRegistrationInfo.push_back(RegisterObjectInfo(class_name, unique_name));
				PushLogger(LOG_TRACE, SYSMD_LB_CORE, "registration user_object name: %s", unique_name);
			}
			else
				PushLogger(LOG_ERROR, SYSMD_LB_CORE, "registration user_object nullptr.");
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
			PushLogger(LOG_TRACE, SYSMD_LB_CORE, "delete user_object name: %s", unique_name);
		}
		else
			PushLogger(LOG_ERROR, SYSMD_LB_CORE, "delete user_object nullptr.");
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