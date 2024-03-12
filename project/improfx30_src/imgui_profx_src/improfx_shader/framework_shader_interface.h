// framework_shader_interface.

#ifndef _FRAMEWORK_SHADER_INTERFACE_H
#define _FRAMEWORK_SHADER_INTERFACE_H

#include "../improfx_log/framework_log.hpp"
#include "../improfx_renderer/framework_imfxgl_resource.hpp"

// shader program loader_mode.
enum ShaderLoaderMode {
	ShaderFilePath  = 1 << 1,
	ShaderScriptStr = 1 << 2
};

// [2024_02_06](1): ImProFx框架着色器(操作映射)接口.
namespace IFC_SHADER { 

	// ImProFX 基础着色器系统接口[20240207].
	class ShaderSystemBase {
	public:
		// shader system shader_program load.
		virtual bool ShaderProgramLoader(const std::string& vert, const std::string& frag, ShaderLoaderMode mode) = 0;
		// tmu sample texture load. "params" x:width, y:height, z:channels.
		// name: shader uniform 1i(sample2D).
		virtual bool SampleTextureFileLoader(const std::string& file, const std::string& name) = 0;
		virtual bool SampleTextureDataLoader(const std::vector<uint8_t>& data, const Vector3T<uint32_t>& params, const std::string& name) = 0;

		// load & update vertex_shader. "matrix" x:mode_mat, y:view_mat, z:proj_mat.
		virtual void VertexMatrixLoader(const Vector3T<FsMatrix4>& matrix) = 0;
		// load & update vertex_buffer.
		virtual bool VertexDataLoader(const std::vector<float>& vertex) = 0;

		// 每帧Uniform变量调用(每帧重复调用新的一帧不保留状态).
		virtual void FrameUniformLoaderFloat(float value, const std::string& name) = 0;
		virtual void FrameUniformLoaderInt32(int32_t value, const std::string& name) = 0;
		virtual void FrameUniformLoaderVec2f(const Vector2T<float>& value, const std::string& name) = 0;

		// texture =bind=> frame_buffer.
		virtual bool FrameBufferBindTexture(ImfxTexture texture) = 0;
		// "params" x:width, y:height, pixel_channels: OGL24+D8 32bit.0
		virtual std::vector<uint8_t> RenderBufferOutput(const Vector2T<uint32_t>& params) = 0;

		// particle system update.
		virtual void _UPDATE_ShaderSystem() = 0;
		virtual ~ShaderSystemBase() = default;
	};
}

#endif