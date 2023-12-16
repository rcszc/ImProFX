// framework_shader_interface.
// [INTERFACE].

#ifndef _FRAMEWORK_SHADER_INTERFACE_H
#define _FRAMEWORK_SHADER_INTERFACE_H
#include "../improfx_log/framework_log.hpp"

// [2023_11_25](1): ImProFx框架图形底层(OGL着色器)接口.
namespace IFC_SHADER {

	enum ScriptLoadmode {
		StringFilePath = 0, // 着色器文件路径.
		StringScript = 1    // 着色器字符串(脚本).
	};

	// 加载着色器程序句柄: script => program_handle
	class ShaderProgram {
	public:
		// load read vertex fragment shader script.
		virtual void load_shader_vs(const char* VsFile, ScriptLoadmode Mod = StringFilePath) = 0;
		virtual void load_shader_fs(const char* FsFile, ScriptLoadmode Mod = StringFilePath) = 0;

		// return program handle.
		virtual uint32_t create_program_handle() = 0;

		// delete vertex fragment shader_handle.
		virtual void delete_shader() = 0;
	};

	// 加载着色器纹理句柄: image src_data => texture_handle [Texture2D].
	class ShaderTexture {
	public:
		// load source data_param: x: width, y:height, z:color_ch.
		virtual void load_source_data(uint8_t* Pointer, const Vector3T<uint32_t>& Param) = 0;

		// get texture handle. [tex_hd + pgm_hd].
		virtual FsTextureHandle get_texture_handle(const uint32_t& Program, const char* Name) = 0;
	};

	// 加载着色器纹理数组句柄: image src_data => texture_handle [Texture3D].
	class ShaderTextureArray {
	public:
		// load source data_param: x: width, y:height, z:color_ch.
		// n * texture_2d => texture_3d (texture_array).
		virtual void pushload_source_data(uint8_t* Pointer, const Vector3T<uint32_t>& Param) = 0;

		// get create texture handle. [tex_hd + pgm_hd].
		virtual FsTextureArrayHandle create_texture_handle(const uint32_t& Program, const char* Name) = 0;
	};

	// 加载着色器顶点数据句柄: data => vao,vbo => model_handle
	class ShaderVertString {
	public:
		// create vertex attribute,buffer. [system-preset].
		virtual uint32_t create_vao_handle() = 0;
		virtual uint32_t create_vbo_handle() = 0;

		// shader location static,dynamic data.
		virtual void load_vertex_static(const uint32_t& Vao, const uint32_t& Vbo, FsVertexGroups Input, FsModelHandle& Handle) = 0;
		virtual void load_vertex_dynamic(const uint32_t& Vao, const uint32_t& Vbo, FsVertexGroups Input, FsModelHandle& Handle) = 0;

		// 动态顶点组更新数据.
		virtual void update_vertex_data(FsVertexGroups Input, FsModelHandle& Handle) = 0;

		// OGL 缓冲区持久映射(Buffer Persistent Mapping)
		// virtual void mapping_vertex_data(vector<float*> Input, FSmodelHanlde& Handle) = 0;
	};

	// 加载着色器帧缓冲句柄: render =bind=> texture
	class ShaderFrameBuffer {
	public:
		// create fbo, texture =bind=> framebuffer.
		virtual void create_bind_framebuffer(const uint32_t& Texture) = 0;

		// get frame_buffer handle.
		virtual uint32_t get_framebuffer_handle() = 0;
	};

	// 删除着色器资源句柄对象: delete opengl_resources object handle
	class ShaderDeleteHandle {
	public:
		virtual void delete_program_handle(uint32_t& program) = 0;        // free shader program.
		virtual void delete_model_handle(FsModelHandle& model) = 0;       // free model(vao,vbo).
		virtual void delete_texture_handle(FsTextureHandle& texture) = 0; // free texture.
		virtual void delete_framebuffer_handle(uint32_t& fbo) = 0;        // free frame buffer.
	};
}

#endif