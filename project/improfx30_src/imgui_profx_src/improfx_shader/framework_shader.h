// framework_shader.

#ifndef _FRAMEWORK_SHADER_H
#define _FRAMEWORK_SHADER_H
#include "framework_shader_interface.h"

#define SYSMD_LB_SHADERSYS "[IM_SHADER]: "

namespace ImShader {

	class ShaderSystem :public IFC_SHADER::ShaderSystemBase, protected IMPROFX_LOW::IMPROFX_DATASET_LLRES {
	protected:
		std::string ShaderProgramItem  = {};
		std::string FrameBuffer        = {};
		std::string FrameBufferTexture = {};

		std::unordered_map<std::string, std::string> SampleTextureArray = {};

		std::string ResGenString();

	public:
		ShaderSystem(const Vector2T<float>& window_size);
		~ShaderSystem();

		bool ShaderProgramLoader(const std::string& vert, const std::string& frag, ShaderLoaderMode mode) override;
		bool SampleTextureFileLoader(const std::string& file, const std::string& name) override;
		bool SampleTextureDataLoader(const std::vector<uint8_t>& data, const Vector3T<uint32_t>& params, const std::string& name) override;

		void VertexMatrixLoader(const Vector3T<FsMatrix4>& matrix) override;
		bool VertexDataLoader(const std::vector<float>& vertex) override;

		void FrameUniformLoaderFloat(float value, const std::string& name) override;
		void FrameUniformLoaderInt32(int32_t value, const std::string& name) override;
		void FrameUniformLoaderVec2f(const Vector2T<float>& value, const std::string& name) override;

		bool FrameBufferBindTexture(ImfxTexture texture) override;
		std::vector<uint8_t> RenderBufferOutput(const Vector2T<uint32_t>& params) override;

		void _UPDATE_ShaderSystem() override;
	};
}

#endif