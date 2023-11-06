// framework_memory.
#include <typeinfo>

#include "framework_memory.h"

using namespace std;
using namespace LOGCONS;

namespace ImDynamicMem {

	size_t vecstr_bytesize(vector<string> input) {
		size_t retsize = NULL;
		for (const auto it : input)
			retsize += it.size();
		return retsize;
	}

	void ImMemory::AllocDynamicMem(MemType type, size_t length) {
		switch (type) {
		case(Float32):   { DyMemoryFloat32.resize(length); break; }
		case(Float64):   { DyMemoryFloat64.resize(length); break; }
		case(Integer32): { DyMemoryInt32.resize(length);   break; }
		case(Integer64): { DyMemoryInt64.resize(length);   break; }
		case(Byte):      { DyMemoryByte.resize(length);    break; }
		case(String):    { DyMemoryString.resize(length);  break; }
		}
	}

	size_t ImMemory::FineSizeDynamicMem(MemType type) {
		size_t dymem_size = NULL;

		switch (type) {
		case(Float32):   { dymem_size = DyMemoryFloat32.size() * sizeof(float);  break; };
		case(Float64):   { dymem_size = DyMemoryFloat64.size() * sizeof(double); break; };
		case(Integer32): { dymem_size = DyMemoryInt32.size() * sizeof(int32_t);  break; };
		case(Integer64): { dymem_size = DyMemoryInt64.size() * sizeof(int64_t);  break; };
		case(Byte):      { dymem_size = DyMemoryByte.size() * sizeof(uint8_t);   break; };
		case(String):    { dymem_size = vecstr_bytesize(DyMemoryString);         break; };
		case(ALLMEM): {
			dymem_size =
				DyMemoryFloat32.size() * sizeof(float) + DyMemoryFloat64.size() * sizeof(double) +
				DyMemoryInt32.size() * sizeof(int32_t) + DyMemoryInt64.size() * sizeof(int64_t) +
				DyMemoryByte.size() * sizeof(uint8_t) + vecstr_bytesize(DyMemoryString);
			break; }
		}
		return dymem_size;
	}

	vector<float>*   ImMemory::FindGetDynamicMemFloat32() { return DyMemoryFloat32.size() == NULL ? nullptr : &DyMemoryFloat32; }
	vector<double>*  ImMemory::FindGetDynamicMemFloat64() { return DyMemoryFloat64.size() == NULL ? nullptr : &DyMemoryFloat64; }
	vector<int32_t>* ImMemory::FindGetDynamicMemInt32()   { return DyMemoryInt32.size() == NULL ? nullptr : &DyMemoryInt32; }
	vector<int64_t>* ImMemory::FindGetDynamicMemInt64()   { return DyMemoryInt64.size() == NULL ? nullptr : &DyMemoryInt64; }
	vector<uint8_t>* ImMemory::FindGetDynamicMemByte()    { return DyMemoryByte.size() == NULL ? nullptr : &DyMemoryByte; }
	vector<string>*  ImMemory::FindGetDynamicMemString()  { return DyMemoryString.size() == NULL ? nullptr : &DyMemoryString; }

	void ImMemory::DeleteDynamicMem(MemType type) {
		switch (type) {
		case(Float32):   { DyMemoryFloat32.clear(); DyMemoryFloat32.shrink_to_fit(); break; };
		case(Float64):   { DyMemoryFloat64.clear(); DyMemoryFloat64.shrink_to_fit(); break; };
		case(Integer32): { DyMemoryInt32.clear(); DyMemoryInt32.shrink_to_fit();     break; };
		case(Integer64): { DyMemoryInt64.clear(); DyMemoryInt64.shrink_to_fit();     break; };
		case(Byte):      { DyMemoryByte.clear(); DyMemoryByte.shrink_to_fit();       break; };
		case(String):    { DyMemoryString.clear(); DyMemoryString.shrink_to_fit();   break; };
		case(ALLMEM): {
			DyMemoryFloat32.clear(); DyMemoryFloat32.shrink_to_fit();
			DyMemoryFloat64.clear(); DyMemoryFloat64.shrink_to_fit();
			DyMemoryInt32.clear();   DyMemoryInt32.shrink_to_fit();
			DyMemoryInt64.clear();   DyMemoryInt64.shrink_to_fit();
			DyMemoryByte.clear();    DyMemoryByte.shrink_to_fit();
			DyMemoryString.clear();  DyMemoryString.shrink_to_fit();
			break;
		}
		}
	}

	void DynamicMemCreateReg(unordered_map<string, ImMemory*> input, const char* key, MemType type, size_t length) {
		ImMemory* memobj = new ImMemory();

		memobj->AllocDynamicMem(type, length);
		input[key] = memobj;

		const type_info& objinfo = typeid(*memobj);
		LogInfoPush(LOG_TRC, SYSTEM_MODULE_LABLE_memory + "register memory: " + key);
	}

	ImMemory* CallDynamicMem(std::unordered_map<std::string, ImMemory*> input, const char* key) {
		return (input.find(key) != input.end()) ? input[key] : nullptr;
	}
}