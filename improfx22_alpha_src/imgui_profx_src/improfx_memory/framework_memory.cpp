// framework_memory.
#include "framework_memory.hpp"

using namespace std;
using namespace LOGCONS;
using namespace IFC_MEMORY;

namespace ImDynamicMem {

	size_t vecstr_bytesize(vector<string> input) {
		size_t retsize = NULL;
		for (const auto it : input)
			retsize += it.size();
		return retsize;
	}

	bool ImMemory::SamplingTimerUpdate(int64_t time, chrono::steady_clock::time_point timer) {
		if (chrono::duration_cast<chrono::microseconds>(chrono::steady_clock::now() - timer).count() >= time) {
			StateSampleTimer = chrono::steady_clock::now();
			return true;
		}
		else
			return false;
	}

	void ImMemory::MemSystemUpdate() {
		if (SamplingTimerUpdate(StateSampleTime, StateSampleTimer)) {
			// clear memory_state.
			MemoryStateSample.PushOperRate = Vector2T<float>();
			MemoryStateSample.EraseOperRate = Vector2T<float>();
			MemoryStateSample.ResizeOperRate = Vector3T<float>();
		}
	}

	void ImMemory::OperMemoryPush(uint8_t* ptr, size_t bytes) {
		if (ptr) {
			U8TdynamicMemory.insert(U8TdynamicMemory.begin(), ptr, ptr + bytes);

			++MemoryStateSample.PushOperRate.vector_x;
			MemoryStateSample.PushOperRate.vector_y += (float)bytes / 1024.0f;
		}
		else
			PushLogger(LOG_WARNING, SYSMD_LB_MEMORY, "failed oper:push dy_memory, ptr = nullptr.");
	}

	void ImMemory::OperMemoryResize(size_t bytes) {
		if (bytes != 0) {
			size_t MemSize = U8TdynamicMemory.size();
			U8TdynamicMemory.resize(bytes);

			if (bytes > MemSize) // resize:Ôö¼Ó.
				MemoryStateSample.ResizeOperRate.vector_y += bytes - MemSize;
			if (bytes < MemSize) // resize:¼õÉÙ.
				MemoryStateSample.ResizeOperRate.vector_z += MemSize - bytes;
			++MemoryStateSample.ResizeOperRate.vector_x;
		}
		else
			PushLogger(LOG_WARNING, SYSMD_LB_MEMORY, "failed oper:resize dy_memory, bytes = 0.");
	}

	void ImMemory::OperMemoryErase(size_t offset, size_t bytes) {
		if (offset + bytes <= U8TdynamicMemory.size() && bytes != 0) {
			// erase vector data.
			auto EraseBegin = U8TdynamicMemory.begin() + offset;
			U8TdynamicMemory.erase(EraseBegin, EraseBegin + bytes);

			++MemoryStateSample.EraseOperRate.vector_x;
			MemoryStateSample.EraseOperRate.vector_y += (float)bytes / 1024.0f;
		}
		else if (bytes == 0)
			PushLogger(LOG_WARNING, SYSMD_LB_MEMORY, "failed oper:erase dy_memory, bytes = 0.");
		else
			PushLogger(LOG_WARNING, SYSMD_LB_MEMORY, "failed oper:erase dy_memory, offset + bytes > size.");
	}

	MemoryState* ImMemory::GetMemoryState() {
		return &MemoryStateSample;
	}

	vector<uint8_t>* ImMemory::PointerDynamicMem() {
		return &U8TdynamicMemory;
	}

	bool RegisterDyMemObject(unordered_map<string, ImMemoryBase*> input, const char* key, size_t bytes, uint32_t sample_time) {
		auto it = input.find(key);
		if (it != input.end()) {
			PushLogger(LOG_WARNING, SYSMD_LB_MEMORY, "failed register memory object, duplicate_key: %s", key);
			return false;
		}
		else {
			ImMemory* MemoryObject = new ImMemory((int64_t)sample_time);
			MemoryObject->PointerDynamicMem()->resize(bytes);

			input[key] = MemoryObject;
			PushLogger(LOG_TRACE, SYSMD_LB_MEMORY, "register memory object: %s", key);
			return true;
		}
	}

	bool DeleteDyMemObject(unordered_map<string, ImMemoryBase*> input, const char* key) {
		auto it = input.find(key);
		if (it != input.end()) {
			input.erase(it);
			PushLogger(LOG_TRACE, SYSMD_LB_MEMORY, "delete memory object: %s", key);
			return true;
		}
		else {
			PushLogger(LOG_WARNING, SYSMD_LB_MEMORY, "failed delete memory object, not found key.");
			return false;
		}
	}

	ImMemoryBase* FindGetDyMem(unordered_map<string, ImMemoryBase*> input, const char* key) {
		return (input.find(key) != input.end()) ? input[key] : nullptr;
	}

	size_t TotalDyMemSizeCount(unordered_map<string, ImMemoryBase*> input) {
		size_t TotalMemorySize = NULL;

		for (auto it = input.begin(); it != input.end(); ++it)
			TotalMemorySize += it->second->PointerDynamicMem()->size();
		return TotalMemorySize;
	}
}