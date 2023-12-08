// framework_shader. [2023_10_11]
// rcsz. version 1.8.0

#ifndef _FRAMEWORK_SHADER_H
#define _FRAMEWORK_SHADER_H

#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/GLU.h>

#include "framework_shader_interface.h"

#define SYSMD_LB_SHADER "[ogl_shader]: "

// 全局低动态/硬件资源管理(VAO,TMU). [thread-safe]
namespace ShaderGlobalRes {

	// vertex attribute object handle [low_dynamic].
	class ResourcesVAO {
	protected:
		std::unordered_map<std::string, uint32_t> LowDynamicVaoHandle = {};
		std::mutex MutexVao = {};
	public:
		void     VaoHandleReg(const std::string& Name, const uint32_t& Handle);
		uint32_t VaoHandleFind(const std::string& Name);
		void     VaoHandleFree();
	};

	// gpu texture mapping unit. 
	class ResourcesTMU {
	protected:
		std::vector<bool> TmuStateFlag = {};
		std::mutex MutexTmu = {};
	public:
		ResourcesTMU(size_t TmuNumber) {
			TmuStateFlag.resize(TmuNumber);
		}
		bool TmuCountAllocTex2D(FsTextureHandle& Handle);
		bool TmuCountAllocTex3D(FsTextureArrayHandle& Handle);

		void TmuCountFreeTex2D(FsTextureHandle& Handle);
		void TmuCountFreeTex3D(FsTextureArrayHandle& Handle);
	};

	extern ResourcesVAO ResVAO;
	extern ResourcesTMU ResTMU;
}

// ******************************** load shader ********************************
// interface: "ShaderProgram"

uint32_t FSGLEW_INIT_SHADER();
uint32_t FSGLEW_INIT_PROGRAM();

// load script + create program.
class FSloadShader :public IFC_SHADER::ShaderProgram {
protected:
	// shader str data.
	const char* source_vertex_shader   = {};
	const char* source_fragment_shader = {};

	// shader index.
	GLuint shader_ver = NULL, shader_frg = NULL;
	GLuint shader_program = FSGLEW_INIT_SHADER();

	bool repeat_compile = false;
	IFC_SHADER::ScriptLoadmode loadmod[2] = {}; // vs.mod, fs.mod

	char* _read_script_file(const char* _file);                      // read shader file.
	bool _shader_compile(const uint32_t& _shaderhandle, const char* _shader); // compile, print errorlog.

public:
	// 重复编译: 如果shader编译失败会再编译一次(垃圾AMD核显老毛病).
	FSloadShader(bool RepeatCompile = false) {
		repeat_compile = RepeatCompile;
		// init glewshader.
		shader_ver = FSGLEW_INIT_SHADER();
		shader_frg = FSGLEW_INIT_SHADER();

		shader_program = FSGLEW_INIT_PROGRAM();
	}
	~FSloadShader() {
		// free shader data.
		if (source_vertex_shader != nullptr && loadmod[0] == IFC_SHADER::StringFilePath)
			delete[] source_vertex_shader;
		if (source_fragment_shader != nullptr && loadmod[1] == IFC_SHADER::StringFilePath)
			delete[] source_fragment_shader;

		source_vertex_shader   = nullptr;
		source_fragment_shader = nullptr;
	};

	// load_shader.
	void load_shader_vs(const char* InVert, IFC_SHADER::ScriptLoadmode Mod = IFC_SHADER::StringFilePath);
	void load_shader_fs(const char* InFrag, IFC_SHADER::ScriptLoadmode Mod = IFC_SHADER::StringFilePath);
	
    // @return uint32_t (shader program handle)
	uint32_t create_program_handle();

	// delete vertex fragment shader.
	void delete_shader();
	/*
	* @param  DataCount (data length[2])
	* @return uint32_t* (0:vert_hd, 1:frag_hd)
	*/
	uint32_t* GET_SHADER_INDEX(size_t& DataCount);
};

// ******************************** load texture ********************************
// interface: "ShaderTexture"

#define FS_CHANNEL_RGB  3 // texture channels 3 bytes.
#define FS_CHANNEL_RGBA 4 // texture channels 4 bytes.

// texture2d "number" TMU count.
void GLEW_TEXTURE_USE2D(const uint32_t& number, FsTextureHandle& handle);
void GLEW_TEXTURE_UNBIND2D();

// texture3d "number" TMU count.
void GLEW_TEXTURE_USE3D(const uint32_t& number, FsTextureArrayHandle& handle);
void GLEW_TEXTURE_UNBIND3D();

// create texture + read image, texture2d.
// read image .jpg .png [RGB][RGBA].
class FSloadTexture :public IFC_SHADER::ShaderTexture {
protected:
	static uint32_t texture_limit[2];
	uint32_t texture_handle = NULL;

public:
	FSloadTexture(const char* ImageFile);
	FSloadTexture() {};
	
	// load source data: param: x: width, y:height, z:color_ch.
	void load_source_data(uint8_t* Pointer, const Vector3T<uint32_t>& Param);

	// get texture.
	FsTextureHandle get_texture_handle(const uint32_t& Program, const char* Name);
	// set load texture_size limit.
	void set_texture_limit(const Vector2T<uint32_t>& TextureLimit);
};

/*
* ======================== ======================== ========================
* interface : "ShaderTextureArray"
* create texture + read image, texture3d.
* read image .jpg .png [RGB][RGBA].
*/
class FSloadTextureArray :public IFC_SHADER::ShaderTextureArray {
protected:
	std::vector<uint8_t> texture_srcdata = {};
	Vector3T<uint32_t> texture_preset_param = {};

	uint32_t texture_handle = NULL;
	uint32_t texture_count = NULL;

	// parameter comparison.
	bool param_comp(const Vector3T<uint32_t>& a, const Vector3T<uint32_t>& b);

public:
	FSloadTextureArray(const uint32_t& width, const uint32_t& height, const uint32_t& channels) {
		texture_preset_param = Vector3T<uint32_t>(width, height, channels);
	}
	// load source data: param: x: width, y:height, z:color_ch.
	void pushload_source_data(uint8_t* Pointer, const Vector3T<uint32_t>& Param);
	void pushload_image_file(const char* ImageFile);

	// create texture => get texture.
	FsTextureArrayHandle create_texture_handle(const uint32_t& Program, const char* Name);
};

// ======================== ======================== ========================
// create texture2d mode.
enum TextureModeType {
	TextureColorRGBA = 32, // texture color RGBA. 32bit [8 * 4]
	TextureColorHDR  = 64  // texture color RGBA. 64bit [16 * 4]
};

// create texture2d.
class FScreateTexture {
protected:
	uint32_t texture_handle = NULL;
public:
	FScreateTexture(TextureModeType TextureType, const Vector2T<float>& TextureSize, const bool& Clamp = false);
	
	// create texture => bind texture => get texture.
	uint32_t get_texture_handle();
	void unbind_texture();
};

// ******************************** load shaderdata ********************************
// interface: "ShaderVertex"

void GLEW_SHADER_DRAWMODEL(FsModelHandle& object_handle, const int& draw_vertexes);
void GLEW_SHADER_USE(const uint32_t& handle);

// opengl glew => loaddata -> gpu.
class FSloadModelData :public IFC_SHADER::ShaderVertex {
protected:
	uint32_t global_vao_begin = NULL;

	void _glew_vertexattr_object(const GLsizei& vertex_length_bytes, const uint32_t& begin);
	// type: false = static, true = dynamic.
	void _glew_vertexbuffer_object(const uint32_t& vao, const uint32_t& vbo, size_t size_bytes, const float* vertex_data, bool type);

public:
	// create vbo,vao. [必须有vbo实例才能创建vao].
	uint32_t create_vao_handle();
	uint32_t create_vbo_handle();

	// static,dynamic vertex data.
	void load_vertex_static(const uint32_t& Vao, const uint32_t& Vbo, FsVertexGroups Input, FsModelHandle& Handle);
	void load_vertex_dynamic(const uint32_t& Vao, const uint32_t& Vbo, FsVertexGroups Input, FsModelHandle& Handle);

	// update dynamic vertex data.
	void update_vertex_data(FsVertexGroups Input, FsModelHandle& Handle);

	// shader data location count begin.
	void set_location_begin(const uint32_t& Begin) {
		global_vao_begin = Begin;
	}
};

// persets data, triangle.
constexpr float ShaderTemplateRect[72] = {
	-10.0f, -10.0f, 0.0f, 1.0f, 0.0f, 0.7f, 1.0f, 0.0f, 0.0f, 0.0f,0.0f,0.0f,
	 10.0f, -10.0f, 0.0f, 1.0f, 0.0f, 0.7f, 1.0f, 1.0f, 0.0f, 0.0f,0.0f,0.0f,
	 10.0f,  10.0f, 0.0f, 1.0f, 0.0f, 0.7f, 1.0f, 1.0f, 1.0f, 0.0f,0.0f,0.0f,

	-10.0f, -10.0f, 0.0f, 1.0f, 0.0f, 0.7f, 1.0f, 0.0f, 0.0f, 0.0f,0.0f,0.0f,
	 10.0f,  10.0f, 0.0f, 1.0f, 0.0f, 0.7f, 1.0f, 1.0f, 1.0f, 0.0f,0.0f,0.0f,
	-10.0f,  10.0f, 0.0f, 1.0f, 0.0f, 0.7f, 1.0f, 0.0f, 1.0f, 0.0f,0.0f,0.0f
};
constexpr size_t ShaderTemplateRectLen = 72;

// ******************************** load FBO ********************************
// interface: "ShaderFrameBuffer"

void GLEW_FBO_BIND(uint32_t& FBO);
void GLEW_FBO_UNBIND();

void GLEW_CLEAR_BUFFER();

class FSloadFrameBuffer :public IFC_SHADER::ShaderFrameBuffer {
protected:
	uint32_t framebuffer_handle = NULL;
public:
	FSloadFrameBuffer();
	// create frame buffer =bind=> texture.
	void create_bind_framebuffer(const uint32_t& Texture);

	// return FBO + unbind FBO.
	uint32_t get_framebuffer_handle();
};

// ******************************** delete ********************************
// interface: "ShaderDeleteHandle"

class FSdeleteHandle :public IFC_SHADER::ShaderDeleteHandle {
public:
	void delete_program_handle(uint32_t& program);        // free shader program.
	void delete_model_handle(FsModelHandle& model);       // free model.
	void delete_texture_handle(FsTextureHandle& texture); // free texture.
	void delete_framebuffer_handle(uint32_t& fbo);        // free frame buffer object.

	void delete_veratt_handle(uint32_t& vao); // free global vao.
};

//******************************** shader Uniform ********************************
// no_interface, shader gobal value(uniform).

class FSloadShaderUniform {
public:
	void uniform_matrix3x3(const uint32_t& program, const char* name, const FsMatrix3& matrix);
	void uniform_matrix4x4(const uint32_t& program, const char* name, const FsMatrix4& matrix);

	void uniform_float(const uint32_t& program, const char* name, const float& value);
	void uniform_vec2(const uint32_t& program, const char* name, const Vector2T<float>& value);
	void uniform_vec3(const uint32_t& program, const char* name, const Vector3T<float>& value);
	void uniform_vec4(const uint32_t& program, const char* name, const Vector4T<float>& value);
	void uniform_integer(const uint32_t& program, const char* name, const int32_t& value);
};

//******************************** load information ********************************
// no_interface.
// command: sysinfo GPU     [获取GPU信息].
// command: sysinfo Attribs [获取"location"最大值].
// command: sysinfo Texture [获取"texture_units"最大值].
// command: sysinfo MEMcpiy [获取显存容量].
// command: sysinfo MEMsize [获取显存余量].

class FSloadInformation {
protected:
	std::unordered_map<std::string, std::function<void(FSloadInformation*)>> HashCmdFunc = {};

	size_t _tempvalue = NULL;
	bool _print_switch = false;

	// interpret instruction.
	void _info_command(const char* _command);

	void _info_get_attribs_max();
	void _info_get_texture_max();
	void _info_get_gpu_memory();
	void _info_get_gpu_memoryuse();
	void _info_get_gpu_parameter();

public:
	FSloadInformation(const char* loadcomm, const bool& printsw = true);
	~FSloadInformation() {};

	size_t get_sizevalue();
};

extern FSloadShaderUniform SFglobalUniform;
extern FSdeleteHandle      SFglobalDeletel;
extern FSloadModelData     SFglobalTransmitData;

// Texture Units number, Texture index, Program index, Uniform name [Sample].
#define FS_ShaderTexture       GLEW_TEXTURE_USE2D
#define FS_ShaderTextureUnBind GLEW_TEXTURE_UNBIND2D

#define FS_ShaderDrawMD GLEW_SHADER_DRAWMODEL // draw vertex group.
#define FS_ShaderEnable GLEW_SHADER_USE       // use shader program.

#define FS_FrameBufferAbove GLEW_FBO_BIND     // 开始FBO操作.
#define FS_FrameBufferBelow GLEW_FBO_UNBIND   // 结束FBO操作(并切换回默认FBO).

#define FS_ClearFrameBuffer GLEW_CLEAR_BUFFER // 清理帧缓冲.

#endif