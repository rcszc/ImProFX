// framework_shader.
#include "framework_shader.h"

using namespace std;
using namespace LOGCONS;
using namespace IFC_SHADER;

/*
* ******************************** load shader ********************************
* opengl glew shader version_max 4.6
*/
#define SHADER_ERROR        "GLcompiler: "
#define SHADER_LOGLEN       512 // log length [char].
#define SHADER_SOURCE_COUNT 1   // read 1 shader file.

uint32_t FSGLEW_INIT_SHADER() { return glewInit(); }
uint32_t FSGLEW_INIT_PROGRAM() { return glCreateProgram(); }

char* FSloadShader::_read_script_file(const char* _file) {
	fstream _read_shaderdata(_file);
	string  _text_datatemp = {};

	char* _return_sourcedata = {};
	char  _chardata          = {};

	if (_read_shaderdata) {
		PushLogger(LOG_INFO, SYSMD_LB_SHADER, "read_shader file: %s", _file);

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
		PushLogger(LOG_ERR, SYSMD_LB_SHADER, "read_shader failed: %s", _file);
	return _return_sourcedata;
}

bool FSloadShader::_shader_compile(const uint32_t& _shaderindex, const char* _label) {
	int32_t shader_log_length = NULL;
	char*   shader_log_info   = nullptr;

	// compile, shader.
	bool    compile_status  = true;
	int32_t compile_success = NULL;

	if (_label != "program") { // program handle.

		glGetShaderiv(_shaderindex, GL_COMPILE_STATUS, &compile_success);
		glGetShaderiv(_shaderindex, GL_INFO_LOG_LENGTH, &shader_log_length);

		if (!compile_success) {
			compile_status = false;

			shader_log_info = new char[shader_log_length];
			glGetShaderInfoLog(_shaderindex, shader_log_length, NULL, (GLchar*)shader_log_info);

			// print error.
			string error_info = SHADER_ERROR;
			if (_label != nullptr)
				error_info += _label;
			PushLogger(LOG_ERR, "SH: ", "%s %s", error_info.c_str(), shader_log_info);

			delete[] shader_log_info;
		}
		else
			PushLogger(LOG_INFO, SYSMD_LB_SHADER, "%s shader compilation succeeded.", _label);
	}
	else if (_label != nullptr) { // shader handle.

		glGetProgramiv(_shaderindex, GL_LINK_STATUS, &compile_success);
		glGetProgramiv(_shaderindex, GL_INFO_LOG_LENGTH, &shader_log_length);

		if (!compile_success) {
			compile_status = false;

			shader_log_info = new char[shader_log_length];
			glGetProgramInfoLog(_shaderindex, shader_log_length, NULL, (GLchar*)shader_log_info);

			// print error.
			string error_info = SHADER_ERROR;
			if (_label != nullptr)
				error_info += _label;
			PushLogger(LOG_ERR, "PM: ", "%s %s", error_info.c_str(), shader_log_info);

			delete[] shader_log_info;
		}
		else
			PushLogger(LOG_INFO, SYSMD_LB_SHADER, "%s program link succeeded.", _label);
	}
	return compile_status;
}

void FSloadShader::load_shader_vs(const char* InVert, ScriptLoadmode Mod) {
	// create shader => compile =loop?=> return.
	// vertex.
	shader_ver = glCreateShader(GL_VERTEX_SHADER);
	{
		loadmod[0] = Mod;
		if (Mod == IFC_SHADER::StringFilePath)
			source_vertex_shader = (char*)_read_script_file(InVert);
		else
			source_vertex_shader = InVert;
		glShaderSource(shader_ver, SHADER_SOURCE_COUNT, &source_vertex_shader, NULL);
		glCompileShader(shader_ver);
	}

	// verts compile loop.
	for (int32_t i = 0; i < repeat_compile + 1; i++) {

		if ((repeat_compile + 1 > 1) && _shader_compile(shader_ver, "vertex"))
			break;
		else if ((i == NULL) && repeat_compile)
			PushLogger(LOG_TRC, SYSMD_LB_SHADER, "shader vs.repeat compile...");
	}
	glAttachShader(shader_program, shader_ver);
}

void FSloadShader::load_shader_fs(const char* InFrag, ScriptLoadmode Mod) {
	// create shader => compile =loop?=> return.
	// fragment.
	shader_frg = glCreateShader(GL_FRAGMENT_SHADER);
	{
		loadmod[1] = Mod;
		if (Mod == StringFilePath)
			source_fragment_shader = (char*)_read_script_file(InFrag);
		else
			source_fragment_shader = InFrag;
		glShaderSource(shader_frg, SHADER_SOURCE_COUNT, &source_fragment_shader, NULL);
		glCompileShader(shader_frg);
	}

	// frags compile loop.
	for (int32_t i = 0; i < repeat_compile + 1; i++) {
		
		if ((repeat_compile + 1 > 1) && _shader_compile(shader_frg, "fragment"))
			break;
		else if ((i == NULL) && repeat_compile)
			PushLogger(LOG_TRC, SYSMD_LB_SHADER, "shader fs.repeat compile...");
	}
	glAttachShader(shader_program, shader_frg);
}

GLuint FSloadShader::create_program_handle() {
	// fragment,vertex => program.
	glLinkProgram(shader_program);
	_shader_compile(shader_program, "program");

	return shader_program;
}

GLuint* FSloadShader::GET_SHADER_INDEX(size_t& DataCount) {
	static GLuint retshader[2] = {};

	DataCount = 2;
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
void GLEW_TEXTURE_USE2D(const uint32_t& number, FsTextureHandle& handle) {
	// use texture2d.
	glActiveTexture(GL_TEXTURE0 + number);           // 激活纹理单元.
	glBindTexture(GL_TEXTURE_2D, handle.texture_hd); // 绑定纹理到当前上下文.

	// texture => shader texture2d uniform.
	SFglobalUniform.uniform_integer(handle.program_hd, handle.uniform_name.c_str(), number);
}

void GLEW_TEXTURE_UNBIND2D() {
	// opengl state unbind.texture2d
	glBindTexture(GL_TEXTURE_2D, NULL);
}

// texture3d "number" TMU count.
void GLEW_TEXTURE_USE3D(const uint32_t& number, FsTextureArrayHandle& handle) {
	// use texture3d.
	glActiveTexture(GL_TEXTURE0 + number);
	glBindTexture(GL_TEXTURE_3D, handle.texture_hd);

	// texture => shader texture3d uniform.
	SFglobalUniform.uniform_integer(handle.program_hd, handle.uniform_name.c_str(), number);
}

void GLEW_TEXTURE_UNBIND3D() {
	// opengl state unbind.texture3d
	glBindTexture(GL_TEXTURE_3D, NULL);
}

constexpr float CLAMP_COLOR[] = { 0.0f, 0.0f, 0.0f, 0.0f };
// gl texture2d param config.
void GL_TEXTURE_CONFIG_2D(const bool& clamp = false) {
	if (clamp) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, CLAMP_COLOR);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

// gl texture3d param config.
void GL_TEXTURE_CONFIG_3D(const bool& clamp = false) {
	if (clamp) {
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	}
	else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, CLAMP_COLOR);
	}
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

// opengl,stb_image load texture. [.jpg.png]
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// stb_image 读取图片像素源数据.
inline uint8_t* read_imagefile_data(const char* file, const uint32_t texture_limit[2], Vector3T<uint32_t>& texture_param) {
	uint8_t* image_srcpixel_data = nullptr;
	int32_t _width = NULL, _height = NULL, _channels = NULL;

	if (filesystem::exists(file)) {
		image_srcpixel_data = stbi_load(file, &_width, &_height, &_channels, NULL);

		PushLogger(LOG_INFO, SYSMD_LB_SHADER, "texture read_image file: %s", file);
		PushLogger(LOG_INFO, SYSMD_LB_SHADER, "texture image size: %d x %d", _width, _height);
		PushLogger(LOG_INFO, SYSMD_LB_SHADER, "texture image color_channel: %d", _channels);

		// limit texture size.
		if ((uint32_t)_width > texture_limit[0] || (uint32_t)_height > texture_limit[1])
			PushLogger(LOG_INFO, SYSMD_LB_SHADER, "texture_size > limit: %u x %u", texture_limit[0], texture_limit[1]);

		texture_param = Vector3T<uint32_t>(_width, _height, _channels);
	}
	else
		PushLogger(LOG_ERR, SYSMD_LB_SHADER, "texture read_image failed: %s", file);
	return image_srcpixel_data;
}

uint32_t FSloadTexture::texture_limit[2] = { 3840,3840 };

FSloadTexture::FSloadTexture(const char* ImageFile) {
	Vector3T<uint32_t> temp_param = {};

	uint8_t* tempptr = read_imagefile_data(ImageFile, texture_limit, temp_param);
	if (tempptr) {
		load_source_data(tempptr, temp_param);
		// free stb_image data.
		stbi_image_free(tempptr);
	}
}

void FSloadTexture::load_source_data(uint8_t* Pointer, const Vector3T<uint32_t>& Param) {
	if (Pointer) {
		glGenTextures(1, &texture_handle);
		glBindTexture(GL_TEXTURE_2D, texture_handle);

		// failed: opengl index < 1.
		if (texture_handle > 0) PushLogger(LOG_INFO, SYSMD_LB_SHADER, "texture_2d create.");
		else                    PushLogger(LOG_WARN, SYSMD_LB_SHADER, "texture_2d failed create.");

		// [弃用项] GLU Libray.
		// gluBuild2DMipmaps(GL_TEXTURE_2D, _nrChannels, _width, _height, GL_RGBA, GL_UNSIGNED_BYTE, GetImageData);

		if (Param.vector_z == FS_CHANNEL_RGBA)
			glTexImage2D(GL_TEXTURE_2D, NULL, Param.vector_z, Param.vector_x, Param.vector_y, NULL, GL_RGBA, GL_UNSIGNED_BYTE, Pointer);
		else if (Param.vector_z == FS_CHANNEL_RGB)
			glTexImage2D(GL_TEXTURE_2D, NULL, Param.vector_z, Param.vector_x, Param.vector_y, NULL, GL_RGB, GL_UNSIGNED_BYTE, Pointer);

		// opengl load texture settings.
		GL_TEXTURE_CONFIG_2D();

		// 生成 mipmap 级贴图.
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, NULL);
	}
	else
		PushLogger(LOG_ERR, SYSMD_LB_SHADER, "texture data_pointer nullptr.");
}

FsTextureHandle FSloadTexture::get_texture_handle(const uint32_t& Program, const char* Name) {
	FsTextureHandle returnindex = FsTextureHandle(texture_handle, Program, NULL, Name);
	return returnindex;
}

void FSloadTexture::set_texture_limit(const Vector2T<uint32_t>& TextureLimit) {
	// load image size_limit.
	if (TextureLimit.vector_x > 0.0f && TextureLimit.vector_y > 0.0f) {
		// soft conversion.[warr]
		texture_limit[0] = TextureLimit.vector_x;
		texture_limit[1] = TextureLimit.vector_x;
	}
	else
		PushLogger(LOG_WARN, SYSMD_LB_SHADER, "texture set limit value > 0.");
}

// ======================== load texture3d ========================
// alloc texture3d memory, load_image, texture2d * n.

bool FSloadTextureArray::param_comp(const Vector3T<uint32_t>& a, const Vector3T<uint32_t>& b) {
	if (a.vector_x == b.vector_x && a.vector_y == b.vector_y && a.vector_z == b.vector_z) 
		return true;
	else 
		return false;
}

void FSloadTextureArray::pushload_source_data(uint8_t* Pointer, const Vector3T<uint32_t>& Param) {
	if (Pointer && param_comp(Param, texture_preset_param)) {
		// push source data.
		texture_srcdata.insert(texture_srcdata.end(), Pointer, Pointer + size_t(Param.vector_x * Param.vector_y * Param.vector_z));
	}
	else if (!Pointer)
		PushLogger(LOG_ERR, SYSMD_LB_SHADER, "texture data_pointer nullptr.");
	else if (!param_comp(Param, texture_preset_param))
		PushLogger(LOG_ERR, SYSMD_LB_SHADER, "texture preset param.");
}

void FSloadTextureArray::pushload_image_file(const char* ImageFile) {
	Vector3T<uint32_t> temp_param = {};
	uint32_t size_limit[2] = { texture_preset_param.vector_x, texture_preset_param.vector_y };

	uint8_t* tempptr = read_imagefile_data(ImageFile, size_limit, temp_param);

	if (tempptr && param_comp(texture_preset_param, texture_preset_param)) {
		// push source data.
		texture_srcdata.insert(
			texture_srcdata.end(), 
			tempptr,
			tempptr + size_t(texture_preset_param.vector_x * texture_preset_param.vector_y * texture_preset_param.vector_z)
		);
	}
	else if (!tempptr)
		PushLogger(LOG_ERR, SYSMD_LB_SHADER, "texture data_pointer nullptr.");
	else if (!param_comp(texture_preset_param, texture_preset_param))
		PushLogger(LOG_ERR, SYSMD_LB_SHADER, "texture preset param.");
}

FsTextureArrayHandle FSloadTextureArray::create_texture_handle(const uint32_t& Program, const char* Name) {
	glGenTextures(1, &texture_handle);
	glBindTexture(GL_TEXTURE_3D, texture_handle);

	if (texture_handle > 0) PushLogger(LOG_INFO, SYSMD_LB_SHADER, "texture_3d create.");
	else                    PushLogger(LOG_WARN, SYSMD_LB_SHADER, "texture_3d failed create.");

	if (texture_preset_param.vector_z == FS_CHANNEL_RGBA)
		glTexImage3D(
			GL_TEXTURE_3D,
			NULL,
			texture_preset_param.vector_z, 
			texture_preset_param.vector_x, 
			texture_preset_param.vector_y, 
			texture_count, 
			NULL,
			GL_RGBA, 
			GL_UNSIGNED_BYTE, texture_srcdata.data()
		);
	else if (texture_preset_param.vector_z == FS_CHANNEL_RGB)
		glTexImage3D(
			GL_TEXTURE_3D,
			NULL, 
			texture_preset_param.vector_z, 
			texture_preset_param.vector_x, 
			texture_preset_param.vector_y, 
			texture_count, 
			NULL, 
			GL_RGB, 
			GL_UNSIGNED_BYTE, 
			texture_srcdata.data()
		);
	GL_TEXTURE_CONFIG_3D();

	glGenerateMipmap(GL_TEXTURE_3D);
	glBindTexture(GL_TEXTURE_3D, NULL);

	return FsTextureArrayHandle(texture_handle, Program, NULL, texture_count, Name);
}

// ======================== create texture ========================
// alloc texture memory, not load_image.

FScreateTexture::FScreateTexture(TextureModeType TextureType, const Vector2T<float>& TextureSize, const bool& Clamp) {
	// create texture + bind.
	glGenTextures(1, &texture_handle);
	glBindTexture(GL_TEXTURE_2D, texture_handle);

	if (TextureType == TextureColorRGBA) {
		glTexImage2D(GL_TEXTURE_2D, NULL, GL_RGBA, (size_t)TextureSize.vector_x, (size_t)TextureSize.vector_y, NULL, GL_RGBA, GL_FLOAT, nullptr);
		PushLogger(LOG_INFO, SYSMD_LB_SHADER, "texture create RGBA.");
	}
	if (TextureType == TextureColorHDR) {
		glTexImage2D(GL_TEXTURE_2D, NULL, GL_RGBA16F, (size_t)TextureSize.vector_x, (size_t)TextureSize.vector_y, NULL, GL_RGBA, GL_FLOAT, nullptr);
		PushLogger(LOG_INFO, SYSMD_LB_SHADER, "texture create HDR_RGBA.");
	}

	GL_TEXTURE_CONFIG_2D(Clamp);
	glGenerateMipmap(GL_TEXTURE_2D);
}

uint32_t FScreateTexture::get_texture_handle() {
	return texture_handle;
}

void FScreateTexture::unbind_texture() {
	// opengl status unbind.texture
	glBindTexture(GL_TEXTURE_2D, NULL);
}

/*
* ******************************** load shaderdata ********************************
* 四边形图元模型. global object class.
*/
void GLEW_SHADER_DRAWMODEL(FsModelHandle& object_handle, const int& draw_vertexes) {
	// render => program.
	glBindVertexArray(object_handle.vao_handle);
	glBindBuffer(GL_ARRAY_BUFFER, object_handle.vbo_handle);
	glDrawArrays(GL_TRIANGLES, NULL, draw_vertexes);
};

void GLEW_SHADER_USE(const uint32_t& handle) {
	glUseProgram(handle);
}

// vertex data => opengl index.
void FSloadModelData::_glew_vertexattr_object(const GLsizei& vertex_length_bytes, const uint32_t& begin) {
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

// vertex data => opengl.
void FSloadModelData::_glew_vertexbuffer_object(const uint32_t& vao, const uint32_t& vbo, size_t size_bytes, const float* vertex_data, bool type) {
	// bind vao,vbo => data => unbind vao,vbo.
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindVertexArray(vao);
	{
		if (type) glBufferData(GL_ARRAY_BUFFER, size_bytes, vertex_data, GL_STATIC_DRAW);
		else      glBufferData(GL_ARRAY_BUFFER, size_bytes, vertex_data, GL_DYNAMIC_DRAW);
	}
	glBindVertexArray(NULL);
	glBindBuffer(GL_ARRAY_BUFFER, NULL);
}

uint32_t FSloadModelData::create_vao_handle() {
	uint32_t vertex_attr_handle = NULL;

	glGenVertexArrays(1, &vertex_attr_handle);
	glBindVertexArray(vertex_attr_handle);

	// system preset datafmt.
	_glew_vertexattr_object(FS_VERTEX_BYTES, global_vao_begin);
	glBindVertexArray(NULL);

	return vertex_attr_handle;
}

uint32_t FSloadModelData::create_vbo_handle() {
	uint32_t vertex_buffer_handle = NULL;
	
	glGenBuffers(1, &vertex_buffer_handle);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_handle);
	
	return vertex_buffer_handle;
}

void FSloadModelData::load_vertex_static(const uint32_t& Vao, const uint32_t& Vbo, FsVertexGroups Input, FsModelHandle& Handle) {
	// model data => gpu vbo, bind vao. "GL_STATIC_DRAW"
	_glew_vertexbuffer_object(Vao, Vbo, Input.VertexDataLen * sizeof(float), Input.VertexData, false);

	Handle.vao_handle = Vao;
	Handle.vbo_handle = Vbo;
	PushLogger(LOG_INFO, SYSMD_LB_SHADER, "data transmit static vertex_group: %u bytes", Input.VertexDataLen * sizeof(float));
}

void FSloadModelData::load_vertex_dynamic(const uint32_t& Vao, const uint32_t& Vbo, FsVertexGroups Input, FsModelHandle& Handle) {
	// model data => gpu vbo, bind vao. "GL_DYNAMIC_DRAW"
	_glew_vertexbuffer_object(Vao, Vbo, Input.VertexDataLen * sizeof(float), Input.VertexData, true);

	Handle.vao_handle = Vao;
	Handle.vbo_handle = Vbo;
	PushLogger(LOG_INFO, SYSMD_LB_SHADER, "data transmit dynamic vertex_group: %u bytes", Input.VertexDataLen * sizeof(float));
}

void FSloadModelData::update_vertex_data(FsVertexGroups Input, FsModelHandle& Handle) {
	// glBufferData 重新分配缓冲区,覆盖全部写入.
	glBindBuffer(GL_ARRAY_BUFFER, Handle.vbo_handle);
	glBufferData(GL_ARRAY_BUFFER, Input.VertexDataLen * sizeof(float), Input.VertexData, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, NULL);
}

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

void FSloadFrameBuffer::create_bind_framebuffer(const uint32_t& Texture) {
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_handle);
	// frame buffer object => texture.
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Texture, NULL);

	// 检查缓冲帧是否完整.
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		PushLogger(LOG_ERR, SYSMD_LB_SHADER, "fbo incomplete frameBuffer!");
}

uint32_t FSloadFrameBuffer::get_framebuffer_handle() {
	// opengl state UnBind.FrameBufferObject
	glBindFramebuffer(GL_FRAMEBUFFER, NULL);
	return framebuffer_handle;
}

/*
* ******************************** Delete ********************************
* 重复删除句柄无害, global object class.
*/
void FSdeleteHandle::delete_program_handle(uint32_t& program) {
	// opengl [GLEW_GET_FUN(__glewDeleteProgram)].
	PushLogger(LOG_INFO, SYSMD_LB_SHADER, "delete shader_program object. ID: %u", program);
	glDeleteProgram(program);
}

void FSdeleteHandle::delete_model_handle(FsModelHandle& modelobject) {
	PushLogger(LOG_INFO, SYSMD_LB_SHADER, "delete vertex_buffer object. ID: u%", modelobject.vbo_handle);
	glDeleteBuffers(1, &modelobject.vbo_handle);
}

void FSdeleteHandle::delete_texture_handle(FsTextureHandle& texture) {
	PushLogger(LOG_TRC, SYSMD_LB_SHADER, "delete texture object. ID: %u", texture.texture_hd);
	glDeleteTextures(1, &texture.texture_hd);
}

void FSdeleteHandle::delete_framebuffer_handle(uint32_t& fbo) {
	PushLogger(LOG_INFO, SYSMD_LB_SHADER, "delete framebuffer object. ID: %u", fbo);
	glDeleteFramebuffers(1, &fbo);
}

void FSdeleteHandle::delete_veratt_handle(uint32_t& vao) {
	glDeleteVertexArrays(1, &vao);
	PushLogger(LOG_INFO, SYSMD_LB_SHADER, "delete vertex_attribute object. ID: %u", vao);
}

/*
* ******************************** shader Uniform ********************************
* global object class.
*/
void FSloadShaderUniform::uniform_matrix3x3(const uint32_t& program, const char* name, const FsMatrix3& matrix) {
	// Maths - Matrix. GCFP32.[3 x 3]. ptr. 
	int32_t uniformLocation = glGetUniformLocation(program, name);
	glUniformMatrix3fv(uniformLocation, 1, false, matrix.matrix);
}

void FSloadShaderUniform::uniform_matrix4x4(const uint32_t& program, const char* name, const FsMatrix4& matrix) {
	// Maths - Matrix. GCFP32.[4 x 4]. ptr. 
	int32_t uniformLocation = glGetUniformLocation(program, name);
	glUniformMatrix4fv(uniformLocation, 1, false, matrix.matrix);
}

void FSloadShaderUniform::uniform_float(const uint32_t& program, const char* name, const float& value) {
	int32_t uniformLocation = glGetUniformLocation(program, name);
	glUniform1f(uniformLocation, value);
}

void FSloadShaderUniform::uniform_vec2(const uint32_t& program, const char* name, const Vector2T<float>& value) {
	int32_t uniformLocation = glGetUniformLocation(program, name);
	glUniform2f(uniformLocation, value.vector_x, value.vector_y);
}

void FSloadShaderUniform::uniform_vec3(const uint32_t& program, const char* name, const Vector3T<float>& value) {
	int32_t uniformLocation = glGetUniformLocation(program, name);
	glUniform3f(uniformLocation, value.vector_x, value.vector_y, value.vector_z);
}

void FSloadShaderUniform::uniform_vec4(const uint32_t& program, const char* name, const Vector4T<float>& value) {
	int32_t uniformLocation = glGetUniformLocation(program, name);
	glUniform4f(uniformLocation, value.vector_x, value.vector_y, value.vector_z, value.vector_w);
}

// GCFP32_int32.value. 
void FSloadShaderUniform::uniform_integer(const uint32_t& program, const char* name, const int32_t& value) {
	int32_t uniformLocation = glGetUniformLocation(program, name);
	glUniform1i(uniformLocation, value);
}

/*
* ******************************** load information ********************************
* opengl api => device_gpu info.
*/
FSloadInformation::FSloadInformation(const char* loadcomm, const bool& printsw) {

	HashCmdFunc["sysinfo platform gpu"]    = std::function<void(FSloadInformation*)>(&FSloadInformation::_info_get_gpu_parameter);
	HashCmdFunc["sysinfo opengl attribs"]  = std::function<void(FSloadInformation*)>(&FSloadInformation::_info_get_attribs_max);
	HashCmdFunc["sysinfo opengl texture"]  = std::function<void(FSloadInformation*)>(&FSloadInformation::_info_get_texture_max);
	HashCmdFunc["sysinfo memory capacity"] = std::function<void(FSloadInformation*)>(&FSloadInformation::_info_get_gpu_memory);
	HashCmdFunc["sysinfo memory size"]     = std::function<void(FSloadInformation*)>(&FSloadInformation::_info_get_gpu_memoryuse);

	_print_switch = printsw;
	_info_command(loadcomm);
}

void FSloadInformation::_info_command(const char* _command) {
	if (HashCmdFunc.find(_command) != HashCmdFunc.end())
		HashCmdFunc[_command](this);
}

void FSloadInformation::_info_get_attribs_max() {
	GLint nrAttributes = NULL;
	GLint nrBuffer = NULL;

	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &nrBuffer);

	_tempvalue = (size_t)nrAttributes;

	if (_print_switch) {
		PushLogger(LOG_INFO, SYSMD_LB_SHADER, "glinfo (vao)attribs_max units: %d entry", nrAttributes);
		PushLogger(LOG_INFO, SYSMD_LB_SHADER, "glinfo (vbo)buffer_max units: %d entry", nrBuffer);
	}
}

void FSloadInformation::_info_get_texture_max() {
	GLint nrTexture = NULL;
	// GL_MAX_TEXTURE_UNITS.       - 基本纹理单元数量(可用于固定管线).
	// GL_MAX_TEXTURE_IMAGE_UNITS. - 可以用于 Shader 的纹理单元数量.
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &nrTexture);
	_tempvalue = (size_t)nrTexture;

	if (_print_switch)
		PushLogger(LOG_INFO, SYSMD_LB_SHADER, "glinfo [tmu]texture_max units: %d entry", nrTexture);
}

void FSloadInformation::_info_get_gpu_memory() {
	GLint MemoryKb = NULL;

	glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &MemoryKb);
	PushLogger(LOG_PERF, SYSMD_LB_SHADER, "glinfo memory[capacity]: %d mib", MemoryKb / 1024);
	_tempvalue = (size_t)MemoryKb;
}

void FSloadInformation::_info_get_gpu_memoryuse() {
	GLint MemoryKb = NULL;

	glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &MemoryKb);
	PushLogger(LOG_PERF, SYSMD_LB_SHADER, "glinfo memory[size]: %d mib", MemoryKb / 1024);
	_tempvalue = (size_t)MemoryKb;
}

void FSloadInformation::_info_get_gpu_parameter() {
	if (_print_switch) {
		PushLogger(LOG_INFO, SYSMD_LB_SHADER, "glinfo gpu supplier: %s", (const char*)glGetString(GL_VENDOR));
		PushLogger(LOG_INFO, SYSMD_LB_SHADER, "glinfo gpu model: %s", (const char*)glGetString(GL_RENDERER));
		PushLogger(LOG_INFO, SYSMD_LB_SHADER, "glinfo version opengl: %s", (const char*)glGetString(GL_VERSION));
		PushLogger(LOG_INFO, SYSMD_LB_SHADER, "glinfo version glsl: %s", (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
	}
}
// getnumber => outvalue.
size_t FSloadInformation::get_sizevalue() { return _tempvalue; };

// global object define.
FSloadShaderUniform SFglobalUniform;
FSdeleteHandle      SFglobalDeletel;
FSloadModelData     SFglobalTransmitData;