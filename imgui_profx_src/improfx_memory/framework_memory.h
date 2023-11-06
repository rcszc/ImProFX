// framework_memory.

#ifndef _FRAMEWORK_MEMORY_H
#define _FRAMEWORK_MEMORY_H
#include "../improfx_log/framework_log.hpp"

#define SYSTEM_MODULE_LABLE_memory std::string("[dy_memory]: ")

namespace ImDynamicMem {
	enum MemType {

		NULLMEM   = -2,
		ALLMEM    = -1,
		Float32   = 0,
		Float64   = 1,
		Integer32 = 2,
		Integer64 = 3,
		Byte      = 4,
		String    = 5
	};

	class ImMemory {
	protected:
		std::vector<float>       DyMemoryFloat32 = {};
		std::vector<double>      DyMemoryFloat64 = {};
		std::vector<int32_t>     DyMemoryInt32   = {};
		std::vector<int64_t>     DyMemoryInt64   = {};
		std::vector<uint8_t>     DyMemoryByte    = {};
		std::vector<std::string> DyMemoryString  = {};

	public:
		void AllocDynamicMem(MemType type, size_t length);
		// size: bytes (not length).
		size_t FineSizeDynamicMem(MemType type);

		std::vector<float>*       FindGetDynamicMemFloat32();
		std::vector<double>*      FindGetDynamicMemFloat64();
		std::vector<int32_t>*     FindGetDynamicMemInt32();
		std::vector<int64_t>*     FindGetDynamicMemInt64();
		std::vector<uint8_t>*     FindGetDynamicMemByte();
		std::vector<std::string>* FindGetDynamicMemString();

		// warning: delete != clear() (clear + shrink_to_fit)
		void DeleteDynamicMem(MemType type);
	};

	void DynamicMemCreateReg(
		std::unordered_map<std::string, ImMemory*> input,
		const char*                                key,
		MemType                                    type = NULLMEM,
		size_t                                     length = NULL
	);

	ImMemory* CallDynamicMem(std::unordered_map<std::string, ImMemory*> input, const char* key);
}

#endif