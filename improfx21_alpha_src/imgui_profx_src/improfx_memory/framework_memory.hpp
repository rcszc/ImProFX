// framework_memory. (此模块我写的纯屎... (没办法如果用std::any后面转换拷贝会牵扯到很多问题.

#ifndef _FRAMEWORK_MEMORY_H
#define _FRAMEWORK_MEMORY_H
#include <type_traits>
#include "framework_memory_interface.h"

#define SYSMD_LB_MEMORY "[im_dymemory]: "

namespace ImDynamicMem {
	namespace Tools {

		template <typename T>
		std::vector<std::vector<T>> Vector1DConvertTo2D(const std::vector<T>& input, size_t x, size_t y) {
			std::vector<std::vector<T>> ResultMat = {};
			size_t VectorLength = x * y;

			if (input.size() != VectorLength) {
				ResultMat = std::vector<std::vector<T>>(x, std::vector<T>(y));
				// convert.
				for (size_t i = 0; i < x; ++i)
					for (size_t j = 0; j < y; ++j)
						ResultMat[i][j] = input[i * y + j];
			}
			else
				LOGCONS::PushLogger(LOG_ERR, SYSMD_LB_MEMORY, "failed tool:'Vector1DConvertTo2D' 1D => 2D param.");
			return ResultMat;
		}

		// warning: data is a matrix.
		template <typename T>
		std::vector<T> Vector2DConvertTo1D(const std::vector<std::vector<T>>& input) {
			std::vector<T> ResultArray = {};
			size_t VectorLength = input.size() * input[0].size();

			ResultArray = std::vector<T>(VectorLength);
			// convert.
			for (size_t i = 0; i < input.size(); ++i)
				for (size_t j = 0; j < input[0].size(); ++j)
					input[i * input[0].size() + j] = ResultArray[i][j];
			return ResultArray;
		}

		template <typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
		std::vector<uint8_t> FloatConvertToBytes(const std::vector<T>& input) {
			size_t InSize = input.size();
			std::vector<uint8_t> ResultBytesArray(InSize * sizeof(T));

			for (size_t i = 0; i < InSize; ++i) {
				float FloatValue = input[i];
				std::memcpy(&ResultBytesArray[i * sizeof(float)], &FloatValue, sizeof(float));
			}
			return ResultBytesArray;
		}

		template <typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
		std::vector<T> BytesConvertToFloat(const std::vector<uint8_t>& input) {
			size_t InSize = input.size();

			if (InSize < sizeof(T)) {
				LOGCONS::PushLogger(LOG_ERR, SYSMD_LB_MEMORY, "failed tool:'BytesConvertToFloat' size < sizeof(T).");
				return std::vector<T>();
			}

			if (InSize % sizeof(T) == 0) {
				std::vector<T> ResultFloatArray(InSize / sizeof(T));

				for (size_t i = 0; i < ResultFloatArray.size(); ++i)
					std::memcpy(&ResultFloatArray[i], &input[i * sizeof(float)], sizeof(float));
				return ResultFloatArray;
			}
			else {
				LOGCONS::PushLogger(LOG_WARN, SYSMD_LB_MEMORY, "failed tool:'BytesConvertToFloat' size % sizeof(T) != 0.");
				return std::vector<T>();
			}
		}
	}

#define DEFAULT_SMP_TIME 5000
	class ImMemory :public IFC_MEMORY::ImMemoryBase {
	protected:
		std::vector<uint8_t> U8TdynamicMemory = {};

		IFC_MEMORY::MemoryState MemoryStateSample = {};
		// sample timer millisecond.
		int64_t StateSampleTime = NULL;
		std::chrono::steady_clock::time_point StateSampleTimer = {};

		bool SamplingTimerUpdate(int64_t time, std::chrono::steady_clock::time_point timer);

	public:
		ImMemory(int64_t smp_time) {
			StateSampleTime = smp_time;
			StateSampleTimer = std::chrono::steady_clock::now();
		}
		// framework system call.
		void MemSystemUpdate() override;

		void OperMemoryPush(uint8_t* ptr, size_t bytes) override;
		void OperMemoryResize(size_t bytes) override;
		void OperMemoryErase(size_t offset, size_t bytes) override;

		IFC_MEMORY::MemoryState* GetMemoryState() override;

		std::vector<uint8_t>* PointerDynamicMem() override;
	};

	bool RegisterDyMemObject(
		std::unordered_map<std::string, IFC_MEMORY::ImMemoryBase*> input,
		const char*                                                key,
		size_t													   bytes       = NULL,
		uint32_t                                                   sample_time = DEFAULT_SMP_TIME
	);

	bool DeleteDyMemObject(std::unordered_map<std::string, IFC_MEMORY::ImMemoryBase*> input, const char* key);

	IFC_MEMORY::ImMemoryBase* FindGetDyMem(std::unordered_map<std::string, IFC_MEMORY::ImMemoryBase*> input, const char* key);
	// 获取所有内存对象内存大小(bytes).
	size_t TotalDyMemSizeCount(std::unordered_map<std::string, IFC_MEMORY::ImMemoryBase*> input);
}

#endif