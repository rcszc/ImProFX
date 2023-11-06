// ShaderFramework_load.
#include <fstream>

#include "framework_shader.h"

using namespace std;
using namespace LOGCONS;

/*
* ******************************** load shader ********************************
* opengl glew shader version_max 4.6
*/
#define SHADER_ERROR        "GLcompiler: "
#define SHADER_LOGLEN       512  // log length [char].
#define SHADER_SOURCE_COUNT 1    // read 1 shader file.

uint32_t FS_GLEW_SHADER_INIT()    { return glewInit(); }
uint32_t FS_GLEW_PROGRAM_CREATE() { return glCreateProgram(); }

void PGC_MSAA() { 
	// 开启 MSAA 抗锯齿.
	// 在 GLFW Config 中也可以开启.
	glEnable(GL_MULTISAMPLE); 
}

char* FSloadShader::_read_shader_file(const char* _file) {
	fstream _read_shaderdata(_file);
	string  _text_datatemp = {};

	char* _return_sourcedata = {};
	char  _chardata          = {};

	if (_read_shaderdata) {
		LogInfoPush(LOG_INFO, SYSTEM_MODULE_LABLE_shader + "read_shader file: " + _file);

		// read source.
		while (_read_shaderdata.get(_chardata)) {
			_text_datatemp.push_back(_chardata);
		}
		// add end flag.
		_text_datatemp.push_back('\0');

		size_t source_length = _text_datatemp.size();
		_return_sourcedata = new char[source_length];

		for (size_t i = 0; i < source_length; ++i)
			_return_sourcedata[i] = _text_datatemp[i];
	}
	else
		LogInfoPush(LOG_ERR, SYSTEM_MODULE_LABLE_shader + "read_shader failed: " + _file);
	return _return_sourcedata;
}

bool FSloadShader::_shader_compile(GLuint _shaderindex, const char* _lable) {

	int32_t shader_log_length = NULL;
	char*   shader_log_info   = nullptr;

	// compile, shader.
	bool    compile_status  = true;
	int32_t compile_success = NULL;

	if (_lable != "program") { // program handle.

		glGetShaderiv(_shaderindex, GL_COMPILE_STATUS, &compile_success);
		glGetShaderiv(_shaderindex, GL_INFO_LOG_LENGTH, &shader_log_length);

		if (!compile_success) {
			compile_status = false;

			shader_log_info = new char[shader_log_length];
			glGetShaderInfoLog(_shaderindex, shader_log_length, NULL, (GLchar*)shader_log_info);

			// print error.
			string error_info = SHADER_ERROR;
			if (_lable != nullptr)
				error_info += _lable;
			LogInfoPush(LOG_ERR, error_info + " " + shader_log_info);

			delete[] shader_log_info;
		}
		else
			LogInfoPush(LOG_INFO, SYSTEM_MODULE_LABLE_shader + _lable + " shader compilation succeeded.");
	}
	else if (_lable != nullptr) { // shader handle.

		glGetProgramiv(_shaderindex, GL_LINK_STATUS, &compile_success);
		glGetProgramiv(_shaderindex, GL_INFO_LOG_LENGTH, &shader_log_length);

		if (!compile_success) {
			compile_status = false;

			shader_log_info = new char[shader_log_length];
			glGetProgramInfoLog(_shaderindex, shader_log_length, NULL, (GLchar*)shader_log_info);

			// print error.
			string error_info = SHADER_ERROR;
			if (_lable != nullptr)
				error_info += _lable;
			LogInfoPush(LOG_ERR, error_info + " " + shader_log_info);

			delete[] shader_log_info;
		}
		else
			LogInfoPush(LOG_INFO, SYSTEM_MODULE_LABLE_shader + _lable + " program link succeeded.");
	}
	return compile_status;
}

void FSloadShader::load_shader_vs(std::string VsFile) {
	// create shader => compile =loop?=> return.
	// vertex.
	shader_ver = glCreateShader(GL_VERTEX_SHADER);
	{
		source_vertex_shader = (char*)_read_shader_file(VsFile.c_str());
		glShaderSource(shader_ver, SHADER_SOURCE_COUNT, &source_vertex_shader, NULL);
		glCompileShader(shader_ver);
	}

	// verts compile loop.
	for (int32_t i = 0; i < repeat_compile + 1; i++) {

		if ((repeat_compile + 1 > 1) && _shader_compile(shader_ver, "vertex"))
			break;
		else if ((i == NULL) && repeat_compile)
			LogInfoPush(LOG_TRC, SYSTEM_MODULE_LABLE_shader + "shader vs.repeat compile...");
	}
	glAttachShader(shader_program, shader_ver);
}

void FSloadShader::load_shader_fs(std::string FsFile) {
	// create shader => compile =loop?=> return.
	// fragment.
	shader_frg = glCreateShader(GL_FRAGMENT_SHADER);
	{
		source_fragment_shader = (char*)_read_shader_file(FsFile.c_str());
		glShaderSource(shader_frg, SHADER_SOURCE_COUNT, &source_fragment_shader, NULL);
		glCompileShader(shader_frg);
	}

	// frags compile loop.
	for (int32_t i = 0; i < repeat_compile + 1; i++) {
		
		if ((repeat_compile + 1 > 1) && _shader_compile(shader_frg, "fragment"))
			break;
		else if ((i == NULL) && repeat_compile)
			LogInfoPush(LOG_TRC, SYSTEM_MODULE_LABLE_shader + "shader fs.repeat compile...");
	}
	glAttachShader(shader_program, shader_frg);
}

GLuint FSloadShader::link_shader_program() {
	// fragment,vertex => program.
	glLinkProgram(shader_program);
	_shader_compile(shader_program, "program");

	return shader_program;
}

GLuint* FSloadShader::GET_SHADER_INDEX(size_t& DataLen) {
	static GLuint retshader[2] = {};

	DataLen = 2;
	retshader[0] = shader_ver;
	retshader[1] = shader_frg;

	return retshader;
}

void FSloadShader::delete_shader() {
	glDeleteShader(shader_ver);
	glDeleteShader(shader_frg);
}

/*
* ******************************** load texture ********************************
* image texture .jpg RGB .png RGBA
*/
#define TEXTURE_INFOHEAD "[Texture]: "

inline bool _TESTFILE(const char* _File) {
	bool _returnstate = false;

	fstream _TestFile(_File);
	if (_TestFile) // Test File Exists.
		_returnstate = true;
	_TestFile.close();

	return _returnstate;
}

void GL_TEXTURE_CONFIG() {

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void GLEW_TEXTURE_USE(uint32_t number, FStextureHandle& handle) {
	// use texture.
	glActiveTexture(GL_TEXTURE0 + number);           // 激活纹理单元.
	glBindTexture(GL_TEXTURE_2D, handle.texture_hd); // 绑定纹理到当前上下文.

	// texture => shader texture uniform.
	SFglobalUniform.sUniform1i(handle.program_hd, handle.uniform_name.c_str(), number);
}

void GLEW_TEXTURE_UNBIND() {
	// opengl state unbind.texture
	glBindTexture(GL_TEXTURE_2D, NULL);
}

// opengl,stb_image load texture. [.jpg.png]
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

uint32_t FSloadTexture::texture_limit[2] = { 4096,4096 };

FSloadTexture::FSloadTexture(string _file) {
	int32_t _width = NULL, _height = NULL, _channels = NULL;

	if (_TESTFILE(_file.c_str())) {
		uint8_t* get_image_data = stbi_load(_file.c_str(), &_width, &_height, &_channels, NULL);

		LogInfoPush(LOG_INFO, SYSTEM_MODULE_LABLE_shader + "texture read_image file: " + _file);
		LogInfoPush(LOG_INFO, SYSTEM_MODULE_LABLE_shader + "texture image width: " + to_string(_width));
		LogInfoPush(LOG_INFO, SYSTEM_MODULE_LABLE_shader + "texture image height: " + to_string(_height));
		LogInfoPush(LOG_INFO, SYSTEM_MODULE_LABLE_shader + "texture image color_channel: " + to_string(_channels));

		// limit texture size.
		if ((uint32_t)_width > texture_limit[0] ||
			(uint32_t)_height > texture_limit[1]
		) {
			LogInfoPush(LOG_INFO, SYSTEM_MODULE_LABLE_shader + "texture texture > limit: " + to_string(texture_limit[0]));
		}

		glGenTextures(1, &texture_handle);
		glBindTexture(GL_TEXTURE_2D, texture_handle);

		if (texture_handle < 1) // opengl index < 1.
			LogInfoPush(LOG_WARN, SYSTEM_MODULE_LABLE_shader + "texture create failed.");

		// [弃用项] GLU.
		// gluBuild2DMipmaps(GL_TEXTURE_2D, _nrChannels, _width, _height, GL_RGBA, GL_UNSIGNED_BYTE, GetImageData);

		if (_channels > FS_CHANNEL_RGB)
			glTexImage2D(GL_TEXTURE_2D, NULL, _channels, _width, _height, NULL, GL_RGBA, GL_UNSIGNED_BYTE, get_image_data);
		else
			glTexImage2D(GL_TEXTURE_2D, NULL, _channels, _width, _height, NULL, GL_RGB, GL_UNSIGNED_BYTE, get_image_data);

		// opengl load texture init.
		GL_TEXTURE_CONFIG();

		// 生成 mipmap 级贴图.
		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, NULL); // 解绑纹理.
		stbi_image_free(get_image_data);    // freeimgdata.
	}
	else
		LogInfoPush(LOG_ERR, SYSTEM_MODULE_LABLE_shader + "texture read_image failed: " + _file);
}

FStextureHandle FSloadTexture::get_texture_handle(uint32_t Program) {
	FStextureHandle returnindex = FStextureHandle(texture_handle, Program, NULL);
	return returnindex;
}

void FSloadTexture::set_texture_limit(Vector2T<uint32_t> SetTexLimit) {
	// load image sizelimit.
	if (SetTexLimit.vector_x > 0.0f && SetTexLimit.vector_y > 0.0f) {
		// soft conversion.[warr]
		texture_limit[0] = SetTexLimit.vector_x;
		texture_limit[1] = SetTexLimit.vector_x;
	}
	else
		LogInfoPush(LOG_WARN, SYSTEM_MODULE_LABLE_shader + "texture set limit value > 0.");
}

// ======================== Create Texture Handle ========================

FScreateTexture::FScreateTexture(uint32_t TexType, Vector2T<float> TextureSize) {

	glGenTextures(1, &texture_index);
	glBindTexture(GL_TEXTURE_2D, texture_index);

	if (TexType == FS_TEXTURE_RGBA) { // RGBA 32bit 1byte * 4.
		glTexImage2D(
			GL_TEXTURE_2D, NULL, GL_RGBA, 
			(size_t)TextureSize.vector_x, (size_t)TextureSize.vector_y, 
			NULL, GL_RGBA, GL_FLOAT, nullptr
		);

		LogInfoPush(LOG_INFO, SYSTEM_MODULE_LABLE_shader + "texture create RGBA.");
	}

	if (TexType == FS_TEXTURE_HDRRGBA) { // RGBA 64bit 2byte * 4.
		glTexImage2D(
			GL_TEXTURE_2D, NULL, GL_RGBA16F,
			(size_t)TextureSize.vector_x, (size_t)TextureSize.vector_y,
			NULL, GL_RGBA, GL_FLOAT, nullptr
		);

		LogInfoPush(LOG_INFO, SYSTEM_MODULE_LABLE_shader + "texture create HDR_RGBA.");
	}

	GL_TEXTURE_CONFIG();
	glGenerateMipmap(GL_TEXTURE_2D);
}

uint32_t FScreateTexture::get_texture_handle() {
	return texture_index;
}

void FScreateTexture::unbind_texture() {
	// opengl status unbind.texture
	glBindTexture(GL_TEXTURE_2D, NULL);
}

/*
* ******************************** load shaderdata ********************************
* 四边形图元模型.
* global object class.
*/

void GLEW_SHADERDRAW_MODEL(FSmodelHanlde& object_handle) {
	// render => program.
	glBindVertexArray(object_handle.vao_handle);
	glDrawArrays(GL_QUADS, NULL, FS_ELEMENT_VERTEX);
};

void GLEW_USESHADER(uint32_t& handle) {
	glUseProgram(handle);
}
// vertex data => opengl index.
inline void FS_GLEW_DataHandle(uint32_t vertex_length_bytes, uint32_t begin) {
	// vertex_data.poscoord 3 * float, bias = 0, Location = 0.
	// vertex_block.begin = 0, vertex_block.end = 2.
	glVertexAttribPointer(begin + 0, FS_VERTEX_COORD, GL_FLOAT, GL_FALSE, vertex_length_bytes,
		(void*)0); // bias_pointer.
	glEnableVertexAttribArray(begin + 0);

	// vertex_data.color 4 * float, bias = 0 + 3, Location = 1.
	// vertex_block.begin = 3, vertex_block.end = 6.
	glVertexAttribPointer(begin + 1, FS_VERTEX_COLOR, GL_FLOAT, GL_FALSE, vertex_length_bytes,
		(void*)(3 * sizeof(float))); // bias_pointer.
	glEnableVertexAttribArray(begin + 1);

	// vertex_data.texture 2 * float, bias = 0 + 3 + 4, Location = 2.
	// vertex_block.begin = 7, vertex_block.end = 9.
	glVertexAttribPointer(begin + 2, FS_VERTEX_TEXUV, GL_FLOAT, GL_FALSE, vertex_length_bytes,
		(void*)(7 * sizeof(float))); // bias_pointer.
	glEnableVertexAttribArray(begin + 2);

	// vertex_data.normal 3 * float, bias = 0 + 3 + 4 + 2, Location = 3.
	// vertex_block.begin = 9, vertex_block.end = 11.
	glVertexAttribPointer(begin + 3, FS_VERTEX_COORD, GL_FLOAT, GL_FALSE, vertex_length_bytes,
		(void*)(9 * sizeof(float))); // bias_pointer.
	glEnableVertexAttribArray(begin + 3);
}

// vertex data => opengl. VAO VBO.
inline void FS_GLEW_DataLoad(uint32_t& VAO, uint32_t& VBO, uint32_t data_length_byte, float* vertex_data, uint32_t begin) {
	// Create VAO, VBO.
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);             // bind vertex.array
	glBindBuffer(GL_ARRAY_BUFFER, VBO); // bind vertex.buffer

	glBufferData(GL_ARRAY_BUFFER, data_length_byte, vertex_data, GL_STATIC_DRAW);
	// "Location" 0~3.
	// data struct: { pos, color, textureUV, normal }.
	FS_GLEW_DataHandle(FS_VERTEX_BYTES, begin);

	glBindVertexArray(NULL); // unbind vertex array.
	glBindBuffer(GL_ARRAY_BUFFER, NULL);
}

FSloadModelData SFglobalTransmitData;

void FSloadModelData::load_model_data(FSdynamic Input, FSmodelHanlde& ObjectHandle, uint32_t Begin) {
	// load_model_data => FS_GLEW_DataLoad -> FS_GLEW_DataHandle.
	FS_GLEW_DataLoad(
		ObjectHandle.vao_handle, 
		ObjectHandle.vbo_handle,
		(uint32_t)Input.ModelDataLength * sizeof(float),
		Input.VertexData, 
		Begin 
	);
	LogInfoPush(LOG_INFO, SYSTEM_MODULE_LABLE_shader + "data transmit vertex_group: " + to_string(Input.ModelDataLength * sizeof(float)) + " bytes");
}

/*
* ******************************** shader Uniform ********************************
* Global Object Class.
*/
// glGetUniformLocation => glUniformMatrix3fv.
void FSloadShaderUniform::sUniformMat3(uint32_t program, const char* name, FSmatrix3 matrix) {
	// Maths - Matrix. GCFP32.[3 x 3]. ptr. 
	int32_t uniformLocation = glGetUniformLocation(program, name);
	glUniformMatrix3fv(uniformLocation, 1, false, matrix.matrix);
}

void FSloadShaderUniform::sUniformMat4(uint32_t program, const char* name, FSmatrix4 matrix) {
	// Maths - Matrix. GCFP32.[4 x 4]. ptr. 
	int32_t uniformLocation = 
		glGetUniformLocation(program, name);
	glUniformMatrix4fv(uniformLocation, 1, false, matrix.matrix);
}

void FSloadShaderUniform::sUniform1f(uint32_t program, const char* name, float value) {
	int32_t uniformLocation = 
		glGetUniformLocation(program, name);
	glUniform1f(uniformLocation, value);
}

void FSloadShaderUniform::sUniform2f(uint32_t program, const char* name, Vector2T<float> value) {
	int32_t uniformLocation = 
		glGetUniformLocation(program, name);
	glUniform2f(uniformLocation, value.vector_x, value.vector_y);
}

void FSloadShaderUniform::sUniform3f(uint32_t program, const char* name, Vector3T<float> value) {
	int32_t uniformLocation = 
		glGetUniformLocation(program, name);
	glUniform3f(uniformLocation, value.vector_x, value.vector_y, value.vector_z);
}

void FSloadShaderUniform::sUniform4f(uint32_t program, const char* name, Vector4T<float> value) {
	int32_t uniformLocation = 
		glGetUniformLocation(program, name);
	glUniform4f(uniformLocation, value.vector_x, value.vector_y, value.vector_z, value.vector_w);
}

// GCFP32_int32.value. 
void FSloadShaderUniform::sUniform1i(uint32_t program, const char* name, int32_t value) {
	int32_t uniformLocation =
		glGetUniformLocation(program, name);
	glUniform1i(uniformLocation, value);
}
FSloadShaderUniform SFglobalUniform;

/*
* ******************************** FrameBufferObject ********************************
* texture =bind=> frame buffer object.
*/
void GLEW_FBO_BIND(uint32_t& FBO) {
	// opengl state bind.frame_buffer_object
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
};
void GLEW_FBO_UNBIND() {
	// opengl state unbind.frame_buffer_object
	// switch to default rendering texture.
	glBindFramebuffer(GL_FRAMEBUFFER, NULL);
	glBindFramebuffer(GL_FRAMEBUFFER, NULL);
};

void GLEW_CLEAR_BUFFER() {
	// clear render buffer.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

FSloadFrameBuffer::FSloadFrameBuffer() {
	// create frame buffer object.
	glGenFramebuffers(1, &framebuffer_handle);
}

void FSloadFrameBuffer::bind_framebuffer(uint32_t Texture) {
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_handle);
	// frame buffer object => texture.
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Texture, NULL);

	// 检查缓冲帧是否完整.
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		LogInfoPush(LOG_ERR, SYSTEM_MODULE_LABLE_shader + "fbo incomplete frameBuffer!");
}

uint32_t FSloadFrameBuffer::get_framebuffer_handle() {
	// opengl state UnBind.FrameBufferObject
	glBindFramebuffer(GL_FRAMEBUFFER, NULL);
	return framebuffer_handle;
}

/*
* ******************************** load information ********************************
* opengl glew => gpu_system.
*/

FSloadInformation::FSloadInformation(const char* loadcomm, bool printsw) {

	HashCmd["sysinfo GPU"]     = std::function<void(FSloadInformation*)>(&FSloadInformation::_info_get_gpu_parameter);
	HashCmd["sysinfo Attribs"] = std::function<void(FSloadInformation*)>(&FSloadInformation::_info_get_attribs_max);
	HashCmd["sysinfo Texture"] = std::function<void(FSloadInformation*)>(&FSloadInformation::_info_get_texture_max);
	HashCmd["sysinfo MEMcpiy"] = std::function<void(FSloadInformation*)>(&FSloadInformation::_info_get_gpu_memory);
	HashCmd["sysinfo MEMsize"] = std::function<void(FSloadInformation*)>(&FSloadInformation::_info_get_gpu_memoryuse);

	_print_switch = printsw;
	_info_command(loadcomm);
}

void FSloadInformation::_info_command(const char* _command) {
	if (HashCmd.find(_command) != HashCmd.end())
		HashCmd[_command](this);
}

void FSloadInformation::_info_get_attribs_max() {
	GLint nrAttributes = NULL;

	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	_tempvalue = (size_t)nrAttributes;

	if (_print_switch)
		LogInfoPush(LOG_INFO, SYSTEM_MODULE_LABLE_shader + "glinfo attribs_max[location]: " + to_string(nrAttributes) + " entry");
}

void FSloadInformation::_info_get_texture_max() {
	GLint nrTexture = NULL;
	// GL_MAX_TEXTURE_UNITS.       - 基本纹理单元数量(可用于固定管线).
	// GL_MAX_TEXTURE_IMAGE_UNITS. - 可以用于 Shader 的纹理单元数量.
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &nrTexture);
	_tempvalue = (size_t)nrTexture;

	if (_print_switch)
		LogInfoPush(LOG_INFO, SYSTEM_MODULE_LABLE_shader + "glinfo texture_max[units]: " + to_string(nrTexture) + " entry");
}

void FSloadInformation::_info_get_gpu_memory() {
	GLint MemoryKb = NULL;

	glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &MemoryKb);
	LogInfoPush(LOG_PERF, SYSTEM_MODULE_LABLE_shader + "glinfo memory[capacity]: " + to_string(MemoryKb / 1024) + " mib");
	_tempvalue = (size_t)MemoryKb;
}

void FSloadInformation::_info_get_gpu_memoryuse() {
	GLint MemoryKb = NULL;

	glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &MemoryKb);
	LogInfoPush(LOG_PERF, SYSTEM_MODULE_LABLE_shader + "glinfo memory[size]: " + to_string(MemoryKb / 1024) + " mib");
	_tempvalue = (size_t)MemoryKb;
}

void FSloadInformation::_info_get_gpu_parameter() {
	if (_print_switch) {
		LogInfoPush(LOG_INFO, SYSTEM_MODULE_LABLE_shader + "glinfo gpu supplier: " + (const char*)glGetString(GL_VENDOR));
		LogInfoPush(LOG_INFO, SYSTEM_MODULE_LABLE_shader + "glinfo gpu model: " + (const char*)glGetString(GL_RENDERER));
		LogInfoPush(LOG_INFO, SYSTEM_MODULE_LABLE_shader + "glinfo version opengl: " + (const char*)glGetString(GL_VERSION));
		LogInfoPush(LOG_INFO, SYSTEM_MODULE_LABLE_shader + "glinfo version glsl: " + (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
	}
}
// getnumber => outvalue.
size_t FSloadInformation::get_sizevalue() { return _tempvalue; };

/*
* ******************************** Delete ********************************
* Global Object Class.
*/
void FSdeleteGLindex::delete_shader_program(uint32_t& program) {
	// opengl [GLEW_GET_FUN(__glewDeleteProgram)].
	LogInfoPush(LOG_INFO, SYSTEM_MODULE_LABLE_shader + "delete shader_program object. ID: " + to_string(program));
	glDeleteProgram(program);
}

void FSdeleteGLindex::delete_model_object(FSmodelHanlde& modelobject) {

	LogInfoPush(LOG_INFO, SYSTEM_MODULE_LABLE_shader + "delete (vao,vbo)_object. ID: " + 
		to_string(modelobject.vao_handle) + "," + to_string(modelobject.vbo_handle)
	);

	glDeleteVertexArrays(1, &modelobject.vao_handle);
	glDeleteBuffers(1, &modelobject.vbo_handle);
}

void FSdeleteGLindex::delete_texture_object(FStextureHandle& texture) {
	// opengl [GLAPI void GLAPIENTRY].
	LogInfoPush(LOG_INFO, SYSTEM_MODULE_LABLE_shader + "delete texture_object. ID: " + to_string(texture.texture_hd));
	glDeleteTextures(1, &texture.texture_hd);
}

void FSdeleteGLindex::delete_framebuffer_object(uint32_t& fbo) {
	// opengl [GLAPI void GLAPIENTRY].
	LogInfoPush(LOG_INFO, SYSTEM_MODULE_LABLE_shader + "delete framebuffer_object. ID: " + to_string(fbo));
	glDeleteFramebuffers(1, &fbo);
}
FSdeleteGLindex SFglobalDeletel;

// ******************************** Perset Model ********************************
// TestCUBE shaderdata. [persets]
// vertex: pos, color, uv, normal. [*4]

float ShaderTemplateRect[48] = {
	-10.0f, -10.0f,0.0f, 1.0f,0.0f,0.7f,1.0f, 0.0f,0.0f, 0.0f,0.0f,0.0f,
	 10.0f, -10.0f,0.0f, 1.0f,0.0f,0.7f,1.0f, 1.0f,0.0f, 0.0f,0.0f,0.0f,
	 10.0f,  10.0f,0.0f, 1.0f,0.0f,0.7f,1.0f, 1.0f,1.0f, 0.0f,0.0f,0.0f,
	-10.0f,  10.0f,0.0f, 1.0f,0.0f,0.7f,1.0f, 0.0f,1.0f, 0.0f,0.0f,0.0f
};
size_t ShaderTemplateRectLen = 48;

float* FBORectangleCapture(Vector2T<float> WindowCoord) {
	static float ShaderWindowRD[48] = {
	-WindowCoord.vector_x, -WindowCoord.vector_y,0.0f, 1.0f,0.0f,0.7f,1.0f, 0.0f,0.0f, 0.0f,0.0f,0.0f,
	 WindowCoord.vector_x, -WindowCoord.vector_y,0.0f, 1.0f,0.0f,0.7f,1.0f, 1.0f,0.0f, 0.0f,0.0f,0.0f,
	 WindowCoord.vector_x,  WindowCoord.vector_y,0.0f, 1.0f,0.0f,0.7f,1.0f, 1.0f,1.0f, 0.0f,0.0f,0.0f,
	-WindowCoord.vector_x,  WindowCoord.vector_y,0.0f, 1.0f,0.0f,0.7f,1.0f, 0.0f,1.0f, 0.0f,0.0f,0.0f
	};
	return ShaderWindowRD;
}
size_t FBORectangleCaptureLen = 48;