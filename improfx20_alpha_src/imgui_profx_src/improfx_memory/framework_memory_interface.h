// framework_memory_interface.

#ifndef _FRAMEWORK_MEMORY_INTERFACE_H
#define _FRAMEWORK_MEMORY_INTERFACE_H
#include "../improfx_log/framework_log.hpp"

// 存储模式: 单类型 & 多类型.
enum MemMode {
	SingPatt = 1,
	MultPatt = 2
};
// 存储类型.
enum MemType {
	NULMEM    = -1,
	Float32   = 0,
	Float64   = 1,
	Integer32 = 2,
	Integer64 = 3,
	Byte      = 4,
	String    = 5
};

// [2023_12_01](1): ImProFx内存对象接口.
namespace IFC_MEMORY {

	// memory_base (常规动态内存)
	class ImMemoryBase {
	public:
		// alloc delete memory, size = length * sizeof(type)
		virtual void AllocDynamicMem(MemType type, const size_t& length) = 0;
		virtual void DeleteDynamicMem(MemType type) = 0;

		// find get size: bytes.
		virtual size_t FineSizeDynamicMem(MemType type) = 0;
		virtual size_t FineSizeTotalDynamicMem() = 0;

		// get memory index.
		virtual std::vector<float>*       FindGetDynamicMemFloat32() = 0;
		virtual std::vector<double>*      FindGetDynamicMemFloat64() = 0;
		virtual std::vector<int32_t>*     FindGetDynamicMemInt32()   = 0;
		virtual std::vector<int64_t>*     FindGetDynamicMemInt64()   = 0;
		virtual std::vector<uint8_t>*     FindGetDynamicMemByte()    = 0;
		virtual std::vector<std::string>* FindGetDynamicMemString()  = 0;
	};

	struct ImgFP32Matrix {
		std::vector<std::vector<float>> MatrixData;

		size_t matrix_x;
		size_t matrix_y;
	};

	// memory_base matrix_3d(矩阵动态内存,用于图像处理)[未开工]
	class ImMemoryBaseMatrix {
	public:
		// alloc matrix memory, size = x * y * layer * sizeof(float)
		virtual void AllocMatrixMem(const size_t& x, const size_t& y, const size_t& layer) = 0;
		// delete matrix memory.
		virtual void DeleteMatrixMemLayer(const size_t& count) = 0;
		virtual void DeleteMatrixMem() = 0;

		virtual void PushMatrix2D(const ImgFP32Matrix& data) = 0;

		// find get size: bytes.
		virtual size_t FineSizeTotalMatrixMem() = 0;
	};
}

#endif