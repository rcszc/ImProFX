// framework_memory_interface.

#ifndef _FRAMEWORK_MEMORY_INTERFACE_H
#define _FRAMEWORK_MEMORY_INTERFACE_H
#include "../improfx_log/framework_log.hpp"

// [2023_12_14](2): ImProFx内存对象接口.
namespace IFC_MEMORY {

	struct MemoryState {
		// x:operation_rate, y:size_value(kib)
		Vector2T<float> PushOperRate;
		Vector2T<float> EraseOperRate;
		// x:operation_rate, y:size_inc(kib), z:size_dec(kib)
		Vector3T<float> ResizeOperRate;
	};

	// memory_base (常规动态内存)
	class ImMemoryBase {
	public:
		virtual void MemSystemUpdate() = 0;

		virtual void OperMemoryPush(uint8_t* ptr, size_t bytes) = 0;
		virtual void OperMemoryResize(size_t bytes) = 0;
		virtual void OperMemoryErase(size_t offset, size_t bytes) = 0;

		virtual MemoryState* GetMemoryState() = 0;

		// get memory pointer.
		virtual std::vector<uint8_t>* PointerDynamicMem() = 0;
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