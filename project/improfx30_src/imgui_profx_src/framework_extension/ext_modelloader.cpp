// ext_modelloader.
#include "ext_modelloader.h"

using namespace std;
using namespace ExtLog;

#define MODULE_MODELLOADER "[MODEL_LOADER]: "

ModelLoaderOBJ::ModelLoaderOBJ(const string& filename) {
    ifstream ReadFile(filename);
    if (ReadFile.is_open()) {
       
        string LinesRead = {};
        while (getline(ReadFile, LinesRead)) {

            istringstream ISS(LinesRead);
            string HeaderToken = {};

            ISS >> HeaderToken;
            if (HeaderToken == "v") {
                Vector3T<float> TempDataVertex = {};
                // model vertex coord x,y,z.
                ISS >> TempDataVertex.vector_x >> TempDataVertex.vector_y >> TempDataVertex.vector_z;

                ModelDataVertex.push_back(TempDataVertex);
                ++ModelDataEntries.vector_x;
            }

            if (HeaderToken == "vt") {
                Vector2T<float> TempDataUV = {};
                // model texture_uv x,y.
                ISS >> TempDataUV.vector_x >> TempDataUV.vector_y;

                ModelDataUV.push_back(TempDataUV);
                ++ModelDataEntries.vector_y;
            }

            if (HeaderToken == "vn") {
                Vector3T<float> TempDataNormal = {};
                // model normal coord x,y,z.
                ISS >> TempDataNormal.vector_x >> TempDataNormal.vector_y >> TempDataNormal.vector_z;

                ModelDataNormal.push_back(TempDataNormal);
                ++ModelDataEntries.vector_z;
            }

            if (HeaderToken == "f") {
                TriangleFaceIndex TempDataIndex = {};
                string IndexToken = {};

                // model triangle_face point 1,2,3.
                for (size_t i = 0; i < 3; ++i) {
                    ISS >> IndexToken;

                    istringstream TokenStream(IndexToken);
                    string VertexIdxStr = "0", UvIdxStr = "0", NormalIdxStr = "0";

                    getline(TokenStream, VertexIdxStr, '/');
                    TempDataIndex.VertexIndex[i] = stoi(VertexIdxStr);

                    if (getline(TokenStream, UvIdxStr, '/'))
                        TempDataIndex.UVIndex[i] = stoi(UvIdxStr);

                    getline(TokenStream, NormalIdxStr, '/');
                    TempDataIndex.NormalIndex[i] = stoi(NormalIdxStr);
                }

                ModelDataIndex.push_back(TempDataIndex);
                ++ModelDataEntries.vector_w;
            }
        }

        ELog(LogInfo, MODULE_MODELLOADER, "loader(obj) open file: %s", filename.c_str());
        ELog(LogInfo, MODULE_MODELLOADER, "model vertex: %u, uv: %u, normal: %u, face: %u", 
            ModelDataEntries.vector_x, ModelDataEntries.vector_y, ModelDataEntries.vector_z, ModelDataEntries.vector_w);
        ReadFile.close();
    }
    else
        ELog(LogError, MODULE_MODELLOADER, "failed loader(obj) open file: %s", filename.c_str());
}

// fmt: pos(3), color(4), uv(2), normal(3).
vector<float> ModelLoaderOBJ::FormatData_IMFX(Vector4T<float> InitColor) {
    vector<float> TempFormatData = {};

    for (const auto& FaceIndex : ModelDataIndex) {
        vector<float> TempTriangleData = {};

        // model_src => convert data => stream data.
        for (size_t i = 0; i < 3; ++i) {
            // vertex_position: 2-float32.
            TempTriangleData.insert(TempTriangleData.end(),
                ModelDataVertex[FaceIndex.VertexIndex[i] - 1].data(), ModelDataVertex[FaceIndex.VertexIndex[i] - 1].data() + 3);
            // vertex_color: 4-float32.
            TempTriangleData.insert(TempTriangleData.end(), InitColor.data(), InitColor.data() + 4);
            // vertex_texture: 2-float32.
            TempTriangleData.insert(TempTriangleData.end(),
                ModelDataUV[FaceIndex.UVIndex[i] - 1].data(), ModelDataUV[FaceIndex.UVIndex[i] - 1].data() + 2);
            // vertex_normal: 3-float32.
            TempTriangleData.insert(TempTriangleData.end(),
                ModelDataNormal[FaceIndex.NormalIndex[i] - 1].data(), ModelDataNormal[FaceIndex.NormalIndex[i] - 1].data() + 3);
        }
        TempFormatData.insert(TempFormatData.end(), TempTriangleData.begin(), TempTriangleData.end());
    }
    ELog(LogInfo, MODULE_MODELLOADER, "loader format_data length: %u", TempFormatData.size());
    return TempFormatData;
}