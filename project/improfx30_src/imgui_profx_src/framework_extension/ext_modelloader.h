// ext_modelloader.

#ifndef _EXT_MODELLOADER_H
#define _EXT_MODELLOADER_H
#include "framework_extension_tools.hpp"

struct TriangleFaceIndex {
	// vec 1,2,3 => triangle points a,b,c.
	size_t VertexIndex[3];
	size_t UVIndex[3];
	size_t NormalIndex[3];
};

class ModelLoaderOBJ {
protected:
	std::vector<Vector3T<float>> ModelDataVertex = {};
	std::vector<Vector2T<float>> ModelDataUV     = {};
	std::vector<Vector3T<float>> ModelDataNormal = {};

	std::vector<TriangleFaceIndex> ModelDataIndex = {};

	// x:ver.entries, y:uv.entries, z:nor.entries, w:face.entries
	Vector4T<size_t> ModelDataEntries = {};
public:
	ModelLoaderOBJ(const std::string& filename);
	ModelLoaderOBJ() {};

	// format data: imfx_vao_0x01
	std::vector<float> FormatData_IMFX(Vector4T<float> InitColor);
};

#endif