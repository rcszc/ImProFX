// framework_shader.
#include "framework_shader.h"

using namespace std;
using namespace LOGCONS;
using namespace IFC_SHADER;

constexpr const char* DefultShaderVS = R"(
#version 460 core
layout (location = 0) in vec3 VertexPos;
layout (location = 1) in vec4 VertexColor;
layout (location = 2) in vec2 VertexTexture;
layout (location = 3) in vec3 VertexNormal;

uniform mat4 MatrixModel;
uniform mat4 MatrixView;
uniform mat4 MatrixProjection;

out vec4 FxColor; 
out vec2 FxCoord;
out vec3 FxNormal;

void main()
{
	gl_Position = MatrixModel * MatrixView * MatrixProjection * vec4(VertexPos, 1);
	FxColor     = VertexColor;
	FxCoord     = VertexTexture;
	FxNormal    = VertexNormal;
}
)";

constexpr const char* DefultShaderFS = R"(
#version 460 core

in vec4 FxColor;
in vec2 FxCoord;
in vec3 FxNormal;

out vec4 FragColor;

void main() 
{
    FragColor = vec4(1.0, 0.0, 0.88, 1.0);
}
)";

namespace ImShader {

	ShaderSystem::ShaderSystem(const Vector2T<float>& window_size) {

	}

	ShaderSystem::~ShaderSystem() {

	}

	std::string ShaderSystem::ResGenString() {
		IMPROFX_GEN_NUMBER GenResID;
		return to_string(GenResID.IPFGEN_GenNumberTime());
	}

	bool ShaderSystem::ShaderProgramLoader(const std::string& vert, const std::string& frag, ShaderLoaderMode mode) {
		IMFX_OGL_MAG::ImFXOGLshader ShaderProcess;
		ShaderProgramItem = ResGenString();

		ShaderProcess.ShaderLoaderVS(DefultShaderVS, StringScript);
		ShaderProcess.ShaderLoaderFS(DefultShaderFS, StringScript);

		if (ShaderProcess.CreateCompileShaders())
			LLRES_Shaders->ResourceStorage(ShaderProgramItem, &ShaderProcess);
	}

	bool ShaderSystem::SampleTextureFileLoader(const std::string& file, const std::string& name) {
		IMFX_OGL_MAG::ImFXOGLtexture TextureProcess;
		string TextureIndexTemp = ResGenString();

		// alloc gpu texture sampler unit.
		TextureProcess.TextureSamplerUnit = LLRES_Samplers->AllocTmuCount();

		if (TextureProcess.CreateTextureLoader(file, LinearFiltering)) {
			// key:name, value:llres_name.
			auto it = SampleTextureArray.find(name);
			if (it == SampleTextureArray.end()) {

				SampleTextureArray[name] = TextureIndexTemp;
				LLRES_Textures->ResourceStorage(TextureIndexTemp, &TextureProcess);
			}
		}
	}

	bool ShaderSystem::SampleTextureDataLoader(const std::vector<uint8_t>& data, const Vector3T<uint32_t>& params, const std::string& name) {

	}

	void ShaderSystem::VertexMatrixLoader(const Vector3T<FsMatrix4>& matrix) {

	}

	bool ShaderSystem::VertexDataLoader(const std::vector<float>& vertex) {

	}

	void ShaderSystem::FrameUniformLoaderFloat(float value, const std::string& name) {

	}

	void ShaderSystem::FrameUniformLoaderInt32(int32_t value, const std::string& name) {

	}

	void ShaderSystem::FrameUniformLoaderVec2f(const Vector2T<float>& value, const std::string& name) {

	}

	bool ShaderSystem::FrameBufferBindTexture(ImfxTexture texture) {

	}

	vector<uint8_t> ShaderSystem::RenderBufferOutput(const Vector2T<uint32_t>& params) {

	}

	void ShaderSystem::_UPDATE_ShaderSystem() {

	}
}