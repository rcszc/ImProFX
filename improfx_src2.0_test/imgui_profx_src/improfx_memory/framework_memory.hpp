// framework_memory. (没办法如果用模板后面会牵扯到很多问题.

#ifndef _FRAMEWORK_MEMORY_H
#define _FRAMEWORK_MEMORY_H
#include "../improfx_log/framework_log.hpp"

#define SYSMD_LB_MEMORY std::string("[dy_memory]: ")

namespace ImDynamicMem {
	namespace Tools {

		template <typename T>
		std::vector<std::vector<T>> Vector1DConvertTo2D(const std::vector<T>& input, size_t x, size_t y) {
			std::vector<std::vector<T>> result = {};
			size_t vectorlength = x * y;

			if (input.size() != vectorlength) {
				result = std::vector<std::vector<T>>(x, std::vector<T>(y));
				// convert.
				for (size_t i = 0; i < x; ++i)
					for (size_t j = 0; j < y; ++j)
						result[i][j] = input[i * y + j];
			}
			else
				LOGCONS::LogInfoPush(LOG_ERR, SYSMD_LB_MEMORY + "failed convert 1D => 2D param.");
			return result;
		}

		// warning: data is a matrix.
		template <typename T>
		std::vector<T> Vector2DConvertTo1D(const std::vector<std::vector<T>>& input) {
			std::vector<T> result = {};
			size_t vectorlength = input.size() * input[0].size();

			result = std::vector<T>(vectorlength);
			// convert.
			for (size_t i = 0; i < input.size(); ++i)
				for (size_t j = 0; j < input[0].size(); ++j)
					input[i * input[0].size() + j] = result[i][j];
			return result;
		}
	}

	// 存储模式: 单类型 & 多类型.
	enum MemMode {
		SingPatt = 1,
		MultPatt = 2
	};
	enum MemType {
		NULMEM    = -1,
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

		MemMode ObejectMode = SingPatt;
		MemType ObejectType = NULMEM; // Mode: SingPatt.

	public:
		ImMemory(MemMode Mode) { ObejectMode = Mode; }

		// resize memory.
		void AllocDynamicMem(MemType type, size_t length);
		// size: bytes (not length).
		size_t FineSizeDynamicMem(MemType type);
		size_t FineSizeTotalDynamicMem();

		// size == 0, return nullptr.
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
		MemType                                    type   = NULMEM,
		size_t                                     length = NULL,
		MemMode                                    mode   = SingPatt
	);

	ImMemory* CallDynamicMem(std::unordered_map<std::string, ImMemory*> input, const char* key);
	// 获取所有内存对象内存大小(bytes).
	size_t CountTotalObjectDynamicMem(std::unordered_map<std::string, ImMemory*> input);
}

#endif