// framework_shader. [2023_10_11]
// rcsz. version 1.6.0

#ifndef _FRAMEWORK_SHADER_H
#define _FRAMEWORK_SHADER_H

#include <functional>
#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/GLU.h>

#include "../improfx_log/framework_log.hpp"

#define SYSTEM_MODULE_LABLE_shader std::string("[ogl_shader]: ")

// 开启MSAA抗锯齿 "glEnable(GL_MULTISAMPLE)".
void PGC_MSAA();

//******************************** load shader ********************************
// opengl glew init [shader, program].

uint32_t FS_GLEW_SHADER_INIT();
uint32_t FS_GLEW_PROGRAM_CREATE();

class FSloadShader {
protected:
	// shader str data.
	const char* source_vertex_shader   = nullptr;
	const char* source_fragment_shader = nullptr;

	// shader index.
	GLuint shader_ver = NULL, shader_frg = NULL;
	GLuint shader_program = FS_GLEW_SHADER_INIT();

	bool repeat_compile;

	char* _read_shader_file(const char* _file);                      // read shader file.
	bool _shader_compile(GLuint _shaderhandle, const char* _shader); // compile, print errorlog.
public:
	// 重复编译: 如果shader编译失败会再编译一次.[垃圾 AMD.GPU 老毛病]

	FSloadShader(bool RepeatCompile = false) {
		repeat_compile = RepeatCompile;
		// init glewshader.
		shader_ver = FS_GLEW_SHADER_INIT();
		shader_frg = FS_GLEW_SHADER_INIT();

		shader_program = FS_GLEW_PROGRAM_CREATE();
	}
	~FSloadShader() {
		// free shader data.
		if (source_vertex_shader   != nullptr) delete[] source_vertex_shader;
		if (source_fragment_shader != nullptr) delete[] source_fragment_shader;

		source_vertex_shader   = nullptr;
		source_fragment_shader = nullptr;
	};

	// load_shader.
	void load_shader_vs(std::string VsFile);
	void load_shader_fs(std::string FsFile);

	/*
	* @param  void
	* @return GLuint (shader program handle)
	*/
	GLuint link_shader_program();
	/*
	* @param  size_t&   (data length)
	* @return GLuint[2] (0:vert 1:frag).
	*/
	GLuint* GET_SHADER_INDEX(size_t& DataLen);

	// delete vertex fragment shader.
	void delete_shader();
};

//******************************** load texture ********************************
// opengl gl glu, stbimg => load image_texture. 

#define FS_CHANNEL_RGB  3 // texture channels BYTE_3.
#define FS_CHANNEL_RGBA 4 // texture channels BYTE_4.

void GL_TEXTURE_CONFIG(); // settings.
void GLEW_TEXTURE_USE(uint32_t number, FStextureHandle& handle);
void GLEW_TEXTURE_UNBIND();

// create texture + read image.
// read image .jpg .png [RGB][RGBA].
class FSloadTexture {
protected:
	static uint32_t texture_limit[2];
	uint32_t texture_handle = NULL;

public:
	FSloadTexture(std::string LoadTexture);
	~FSloadTexture() {};

	void set_texture_limit(Vector2T<uint32_t> SetTexLimit);
	// load texture => get texture.
	FStextureHandle get_texture_handle(uint32_t Program);
};

#define FS_TEXTURE_RGBA    32 // texture color RGBA. 32bit [8 * 4]
#define FS_TEXTURE_HDRRGBA 64 // texture color RGBA. 64bit [16 * 4]

// create texture 2D.
class FScreateTexture {
protected:
	uint32_t texture_index = NULL;

public:
	FScreateTexture(uint32_t TexType, Vector2T<float> TextureSize);
	~FScreateTexture() {};

	uint32_t get_texture_handle();
	void unbind_texture();
};

//******************************** load shaderdata ********************************
// rectangle model.

void GLEW_SHADERDRAW_MODEL(FSmodelHanlde& object_handle);
void GLEW_USESHADER(uint32_t& handle);

// opengl glew => loaddata -> gpu.
class FSloadModelData {
public:
	// create object.
	// location end = begin + 4, [begin, begin + 4].
	void load_model_data(FSdynamic Input, FSmodelHanlde& ObjectHandle, uint32_t Begin);
};
extern FSloadModelData SFglobalTransmitData;

// persets data.
extern float  ShaderTemplateRect[(FS_VERTEX_LENGTH) * 4];
extern size_t ShaderTemplateRectLen;

// 用于 FBO 渲染到整个屏幕纹理.
// 平面矩形捕获缓冲.
float* FBORectangleCapture(Vector2T<float> WindowCoord);
extern size_t FBORectangleCaptureLen;

//******************************** load FBO ********************************
// frame buffer object.

void GLEW_FBO_BIND(uint32_t& FBO);
void GLEW_FBO_UNBIND();
void GLEW_CLEAR_BUFFER();

class FSloadFrameBuffer {
protected:
	uint32_t framebuffer_handle = NULL;
public:
	FSloadFrameBuffer();
	~FSloadFrameBuffer() {};

	// 将FBO绑定到Texture.
	void bind_framebuffer(uint32_t Texture);

	// return FBO + UnBind FBO.
	uint32_t get_framebuffer_handle();
};

//******************************** shader Uniform ********************************
// shader gobal value(uniform).

class FSloadShaderUniform {
public:
	void sUniformMat3(uint32_t program, const char* name, FSmatrix3 matrix);
	void sUniformMat4(uint32_t program, const char* name, FSmatrix4 matrix);

	void sUniform1f(uint32_t program, const char* name, float value);
	void sUniform2f(uint32_t program, const char* name, Vector2T<float> value);
	void sUniform3f(uint32_t program, const char* name, Vector3T<float> value);
	void sUniform4f(uint32_t program, const char* name, Vector4T<float> value);
	void sUniform1i(uint32_t program, const char* name, int32_t value);
};
extern FSloadShaderUniform SFglobalUniform;

//******************************** load information ********************************
// command: sysinfo GPU     [获取GPU信息].
// command: sysinfo Attribs [获取"location"最大值].
// command: sysinfo Texture [获取"texture_units"最大值].
// command: sysinfo MEMcpiy [获取显存容量].
// command: sysinfo MEMsize [获取显存余量].

class FSloadInformation {
protected:
	std::unordered_map<std::string, std::function<void(FSloadInformation*)>> HashCmd = {};

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
	FSloadInformation(const char* loadcomm, bool printsw = true);
	~FSloadInformation() {};

	size_t get_sizevalue();
};

//******************************** delete ********************************

class FSdeleteGLindex {
public:
	void delete_shader_program(uint32_t& program);       // free shader program.
	void delete_model_object(FSmodelHanlde& model);       // free model.
	void delete_texture_object(FStextureHandle& texture); // free texture.
	void delete_framebuffer_object(uint32_t& fbo);       // free frame buffer object.
};
extern FSdeleteGLindex SFglobalDeletel;

// Texture Units number, Texture index, Program index, Uniform name [Sample].
#define FS_ShaderTexture       GLEW_TEXTURE_USE
#define FS_ShaderTextureUnBind GLEW_TEXTURE_UNBIND

#define FS_ShaderDrawMD GLEW_SHADERDRAW_MODEL // draw model.
#define FS_ShaderEnable GLEW_USESHADER        // use shader program.

#define FS_FrameBufferAbove GLEW_FBO_BIND   // 开始FBO操作.
#define FS_FrameBufferBelow GLEW_FBO_UNBIND // 结束FBO操作(并切换回默认FBO).

#define FS_ClearFrameBuffer GLEW_CLEAR_BUFFER // 清理帧缓冲.

#endif