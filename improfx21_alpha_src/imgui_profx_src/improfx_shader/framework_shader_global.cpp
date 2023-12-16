// framework_shader_global. [框架着色器全局资源]
#include "framework_shader.h"

using namespace std;
using namespace LOGCONS;
using namespace IFC_SHADER;

namespace ShaderGlobalRes {

	void ResourcesVAO::VaoHandleReg(const string& Name, const uint32_t& Handle) {
		lock_guard<mutex> Lock(MutexVao);

		auto it = LowDynamicVaoHandle.find(Name);
		if (it != LowDynamicVaoHandle.end())
			PushLogger(LOG_WARN, SYSMD_LB_SHADER, "global_res vao name conflict.");
		else
			LowDynamicVaoHandle[Name] = Handle;
	}

	uint32_t ResourcesVAO::VaoHandleFind(const string& Name) {
		lock_guard<mutex> Lock(MutexVao);
		return (LowDynamicVaoHandle.find(Name) != LowDynamicVaoHandle.end()) ? LowDynamicVaoHandle[Name] : NULL;
	}

	void ResourcesVAO::VaoHandleFree() {
		lock_guard<mutex> Lock(MutexVao);
		for (auto it = LowDynamicVaoHandle.begin(); it != LowDynamicVaoHandle.end(); ++it)
			SFglobalDeletel.delete_veratt_handle(it->second);
	}

	inline bool find_tmu_stateflag(vector<bool>& data, mutex& mtx, uint32_t& count) {
		// false: TMU资源耗尽.
		bool idle_resources = false;

		lock_guard<mutex> lock(mtx);
		for (size_t i = 0; i < data.size(); ++i) {
			if (data[i] == false) {
				data[i] = true;
				idle_resources = true;
				count = (uint32_t)i;
				break;
			}
		}
		return idle_resources;
	}

	bool ResourcesTMU::TmuCountAllocTex2D(FsTextureHandle& Handle) {
		uint32_t temp_count = NULL;

		bool idelflag = find_tmu_stateflag(TmuStateFlag, MutexTmu, temp_count);
		if (!idelflag)
			PushLogger(LOG_WARN, SYSMD_LB_SHADER, "global_res tmu texture2d_alloc idle = 0.");
		else
			PushLogger(LOG_TRC, SYSMD_LB_SHADER, "global_res tmu texture2d_alloc count: %u", temp_count);
		Handle.SamplerCount = temp_count;

		return idelflag;
	}

	bool ResourcesTMU::TmuCountAllocTex3D(FsTextureArrayHandle& Handle) {
		uint32_t temp_count = NULL;

		bool idelflag = find_tmu_stateflag(TmuStateFlag, MutexTmu, temp_count);
		if (!idelflag)
			PushLogger(LOG_WARN, SYSMD_LB_SHADER, "global_res tmu texture3d_alloc idle = 0.");
		else
			PushLogger(LOG_TRC, SYSMD_LB_SHADER, "global_res tmu texture3d_alloc count: %u", temp_count);
		Handle.SamplerCount = temp_count;

		return idelflag;
	}

	void ResourcesTMU::TmuCountFreeTex2D(FsTextureHandle& Handle) {
		lock_guard<mutex> lock(MutexTmu);
		if (Handle.SamplerCount >= TmuStateFlag.size())
			PushLogger(LOG_ERR, SYSMD_LB_SHADER, "global_res tmu texture2d_free count > tmu.");
		else {
			TmuStateFlag[(size_t)Handle.SamplerCount] = false;
			PushLogger(LOG_TRC, SYSMD_LB_SHADER, "global_res tmu texture2d_free count: %u", Handle.SamplerCount);
		}
	}

	void ResourcesTMU::TmuCountFreeTex3D(FsTextureArrayHandle& Handle) {
		lock_guard<mutex> lock(MutexTmu);
		if (Handle.SamplerCount >= TmuStateFlag.size())
			PushLogger(LOG_ERR, SYSMD_LB_SHADER, "global_res tmu texture3d_free count > tmu.");
		else {
			TmuStateFlag[(size_t)Handle.SamplerCount] = false;
			PushLogger(LOG_TRC, SYSMD_LB_SHADER, "global_res tmu texture3d_free count: %u" + Handle.SamplerCount);
		}
	}

	uint32_t ResourcesTMU::GetTmuCount() {
		uint32_t return_count = NULL;
		lock_guard<mutex> lock(MutexTmu);
		{
			for (const auto& item : TmuStateFlag)
				if (item)
					++return_count;
		}
		return return_count;
	}

	ResourcesVAO ResVAO;
	ResourcesTMU ResTMU(32);
}