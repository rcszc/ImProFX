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

	bool memory_operations(MemMode mode, MemType src_type, MemType type, const char* oper) {
		bool returnexeflag = true;
		if (mode == SingPatt) {
			if (src_type == type)
				returnexeflag = true;
			else
				PushLogger(LOG_ERR, SYSMD_LB_MEMORY, "failed mode: 'SingPatt' type error, oper: %s", oper);
		}
		else
			returnexeflag = true;
		return returnexeflag;
	}

	void ImMemory::AllocDynamicMem(MemType type, const size_t& length) {
		if (memory_operations(ObejectMode, ObejectType, type, "alloc")) {
			switch (type) {
			case(Float32):   { DyMemoryFloat32.resize(length); break; }
			case(Float64):   { DyMemoryFloat64.resize(length); break; }
			case(Integer32): { DyMemoryInt32.resize(length);   break; }
			case(Integer64): { DyMemoryInt64.resize(length);   break; }
			case(Byte):      { DyMemoryByte.resize(length);    break; }
			case(String):    { DyMemoryString.resize(length);  break; }
			}
		}
	}

	void ImMemory::DeleteDynamicMem(MemType type) {
		if (memory_operations(ObejectMode, ObejectType, type, "delete")) {
			switch (type) {
			case(Float32):   { DyMemoryFloat32.clear(); DyMemoryFloat32.shrink_to_fit(); break; };
			case(Float64):   { DyMemoryFloat64.clear(); DyMemoryFloat64.shrink_to_fit(); break; };
			case(Integer32): { DyMemoryInt32.clear(); DyMemoryInt32.shrink_to_fit();     break; };
			case(Integer64): { DyMemoryInt64.clear(); DyMemoryInt64.shrink_to_fit();     break; };
			case(Byte):		 { DyMemoryByte.clear(); DyMemoryByte.shrink_to_fit();       break; };
			case(String):    { DyMemoryString.clear(); DyMemoryString.shrink_to_fit();   break; };
			}
		}
	}

	size_t ImMemory::FineSizeDynamicMem(MemType type) {
		size_t dymem_size = NULL;

		if (memory_operations(ObejectMode, ObejectType, type, "findsize")) {
			switch (type) {
			case(Float32):   { dymem_size = DyMemoryFloat32.size() * sizeof(float);  break; };
			case(Float64):   { dymem_size = DyMemoryFloat64.size() * sizeof(double); break; };
			case(Integer32): { dymem_size = DyMemoryInt32.size() * sizeof(int32_t);  break; };
			case(Integer64): { dymem_size = DyMemoryInt64.size() * sizeof(int64_t);  break; };
			case(Byte):      { dymem_size = DyMemoryByte.size() * sizeof(uint8_t);   break; };
			case(String):    { dymem_size = vecstr_bytesize(DyMemoryString);         break; };
			}
		}
		return dymem_size;
	}

	size_t ImMemory::FineSizeTotalDynamicMem() {
		return DyMemoryFloat32.size() * sizeof(float) + DyMemoryFloat64.size() * sizeof(double) + DyMemoryInt32.size() * sizeof(int32_t) +
			DyMemoryInt64.size() * sizeof(int64_t) + DyMemoryByte.size() * sizeof(uint8_t) + vecstr_bytesize(DyMemoryString);
	}

	vector<float>*   ImMemory::FindGetDynamicMemFloat32() { return DyMemoryFloat32.size() == NULL ? nullptr : &DyMemoryFloat32; }
	vector<double>*  ImMemory::FindGetDynamicMemFloat64() { return DyMemoryFloat64.size() == NULL ? nullptr : &DyMemoryFloat64; }
	vector<int32_t>* ImMemory::FindGetDynamicMemInt32()   { return DyMemoryInt32.size() == NULL ? nullptr : &DyMemoryInt32; }
	vector<int64_t>* ImMemory::FindGetDynamicMemInt64()   { return DyMemoryInt64.size() == NULL ? nullptr : &DyMemoryInt64; }
	vector<uint8_t>* ImMemory::FindGetDynamicMemByte()    { return DyMemoryByte.size() == NULL ? nullptr : &DyMemoryByte; }
	vector<string>*  ImMemory::FindGetDynamicMemString()  { return DyMemoryString.size() == NULL ? nullptr : &DyMemoryString; }

	bool DynamicMemObjectRegister(unordered_map<string, ImMemoryBase*> input, const char* key, MemType type, size_t length, MemMode mode) {
		auto it = input.find(key);
		if (it != input.end()) {
			PushLogger(LOG_WARN, SYSMD_LB_MEMORY, "failed register memory object, duplicate_key: %s", key);
			return false;
		}
		else {
			ImMemory* memobj = new ImMemory(mode);
			memobj->AllocDynamicMem(type, length);

			input[key] = memobj;
			PushLogger(LOG_TRC, SYSMD_LB_MEMORY, "register memory object: %s", key);
			return true;
		}
	}

	bool DynamicMemObjectDelete(unordered_map<string, ImMemoryBase*> input, const char* key) {
		auto it = input.find(key);
		if (it != input.end()) {
			input.erase(it);
			PushLogger(LOG_TRC, SYSMD_LB_MEMORY, "delete memory object: %s", key);
			return true;
		}
		else {
			PushLogger(LOG_WARN, SYSMD_LB_MEMORY, "failed delete memory object, not found key.");
			return false;
		}
	}

	ImMemoryBase* CallDynamicMem(unordered_map<string, ImMemoryBase*> input, const char* key) {
		return (input.find(key) != input.end()) ? input[key] : nullptr;
	}

	size_t CountTotalObjectDynamicMem(unordered_map<string, ImMemoryBase*> input) {
		size_t memorysize = NULL;

		for (auto it = input.begin(); it != input.end(); ++it)
			memorysize += it->second->FineSizeTotalDynamicMem();
		return memorysize;
	}
}