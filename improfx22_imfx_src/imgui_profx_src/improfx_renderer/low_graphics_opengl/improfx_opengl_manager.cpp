// improfx_opengl_manager.
#include "improfx_opengl.hpp"

RendererLogger ImFXOGLsystemLogger::ImFXLog = nullptr;

namespace IMFX_OGL_VERATT {
	// OpenGL 顶点属性预设.
	void VERTEX_ATT_PRESET_1(const GLsizei& VER_LEN_BYTES, const uint32_t& BEGIN_COUNT) {
		// vertex_data.poscoord 3 * float, bias = 0, Location = 0.
		// vertex_block.begin = 0, vertex_block.end = 2.
		glVertexAttribPointer(BEGIN_COUNT + 0, FS_VERTEX_COORD, GL_FLOAT, GL_FALSE, VER_LEN_BYTES,
			(void*)0); // bias_pointer.
		glEnableVertexAttribArray(BEGIN_COUNT + 0);

		// vertex_data.color 4 * float, bias = 0 + 3, Location = 1.
		// vertex_block.begin = 3, vertex_block.end = 6.
		glVertexAttribPointer(BEGIN_COUNT + 1, FS_VERTEX_COLOR, GL_FLOAT, GL_FALSE, VER_LEN_BYTES,
			(void*)(3 * sizeof(float))); // bias_pointer.
		glEnableVertexAttribArray(BEGIN_COUNT + 1);

		// vertex_data.texture 2 * float, bias = 0 + 3 + 4, Location = 2.
		// vertex_block.begin = 7, vertex_block.end = 9.
		glVertexAttribPointer(BEGIN_COUNT + 2, FS_VERTEX_TEXUV, GL_FLOAT, GL_FALSE, VER_LEN_BYTES,
			(void*)(7 * sizeof(float))); // bias_pointer.
		glEnableVertexAttribArray(BEGIN_COUNT + 2);

		// vertex_data.normal 3 * float, bias = 0 + 3 + 4 + 2, Location = 3.
		// vertex_block.begin = 9, vertex_block.end = 11.
		glVertexAttribPointer(BEGIN_COUNT + 3, FS_VERTEX_COORD, GL_FLOAT, GL_FALSE, VER_LEN_BYTES,
			(void*)(9 * sizeof(float))); // bias_pointer.
		glEnableVertexAttribArray(BEGIN_COUNT + 3);
	}
}

namespace IMFX_OGL_MAG {
	
	INIT_RETURN ImFXOGLinit::RendererInit(INIT_PARAMETERS init_param) {
		return glewInit();
	}

	void ImFXOGLinit::LoggerFunc(RendererLogger function) {
		if (function) {
			ImFXLog = function;
			ImFXLog(LOG_TRACE, IMFX_OGLMAG_LABEL, "infx_logger function_ptr: %u", (uintptr_t)function);
		}
	}

	/*
	* **************************************** ShaderProgram ****************************************
	* OpenGL 4.6 GLSL vertex,fragment "#version 460 core"
	* Update: 2023.12.30 RCSZ.
	*/

	std::string ImFXOGLshader::TextFileLoader(const std::string& file) {
		std::ifstream FileLoader(file);

		if (FileLoader.is_open()) {
			// calc file size.
			FileLoader.seekg(0, std::ios::end);
			size_t FileSize = (size_t)FileLoader.tellg();
			FileLoader.seekg(0, std::ios::beg);

			// read string data.
			std::string FileContent((std::istreambuf_iterator<char>(FileLoader)), std::istreambuf_iterator<char>());

			ImFXLog(LOG_INFO, IMFX_OGLMAG_LABEL, "shader script_loader read: %s read_size: %u", file.c_str(), FileSize);
			return FileContent;
		}
		else {
			ImFXLog(LOG_ERROR, IMFX_OGLMAG_LABEL, "shader script_loader failed open file: %s", file.c_str());
			return std::string("");
		}
	}

	bool ImFXOGLshader::CompilationStatus(const uint32_t& shader, const std::string& label) {
		// gl_func ret stat, error_log size.
		int32_t CompileSuccess = NULL, ShaderLogLength = NULL;

		bool CompileStatus = true;
		char* ShaderLogInfo = nullptr;

		if (label != "program") { // shader handle.

			glGetShaderiv(shader, GL_COMPILE_STATUS, &CompileSuccess);
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &ShaderLogLength);

			if (!CompileSuccess) {
				// program link failed.
				ShaderLogInfo = new char[ShaderLogLength];
				glGetShaderInfoLog(shader, ShaderLogLength, NULL, (GLchar*)ShaderLogInfo);

				// print error.
				std::string ErrorInfo = "GLcompiler: " + label;
				ImFXLog(LOG_ERROR, "Shader: ", "%s %s", ErrorInfo.c_str(), ShaderLogInfo);

				delete[] ShaderLogInfo;
			}
			else
				ImFXLog(LOG_INFO, IMFX_OGLMAG_LABEL, "%s shader compilation succeeded.", label);
		}
		else if (!label.empty()) { // program handle.

			glGetProgramiv(shader, GL_LINK_STATUS, &CompileSuccess);
			glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &ShaderLogLength);

			if (!CompileSuccess) {
				// shader compilation failed.
				ShaderLogInfo = new char[ShaderLogLength];
				glGetProgramInfoLog(shader, ShaderLogLength, NULL, (GLchar*)ShaderLogInfo);

				// print error.
				std::string ErrorInfo = "GLlink: " + label;
				ImFXLog(LOG_ERROR, "Program: ", "%s %s", ErrorInfo.c_str(), ShaderLogInfo);

				delete[] ShaderLogInfo;
			}
			else
				ImFXLog(LOG_INFO, IMFX_OGLMAG_LABEL, "%s program link succeeded.", label);
		}
		return (bool)CompileSuccess;
	}

#define SHADER_SCRIPT_COUNT 1

	void ImFXOGLshader::ShaderLoaderVS(const std::string& vs, ScriptReadMode mode) {
		// vertex shader.
		ShaderVS = glCreateShader(GL_VERTEX_SHADER);
		if (ShaderVS) {
			if (mode == StringFilepath)
				ShaderSrcVS = TextFileLoader(vs);
			else
				ShaderSrcVS = vs;
			const char* VssrcstrTemp = ShaderSrcVS.c_str();
			// read shader script_source => compile.
			glShaderSource(ShaderVS, SHADER_SCRIPT_COUNT, &VssrcstrTemp, NULL);
			glCompileShader(ShaderVS);

			if (!CompilationStatus(ShaderVS, "vertex"))
				ReturnResFlag = DEFRES_FLAG_INVALID;

			glAttachShader(ShaderProgram, ShaderVS);
		}
		else
			ImFXLog(LOG_ERROR, IMFX_OGLMAG_LABEL, "failed create vertex_shader handle.");
	}

	void ImFXOGLshader::ShaderLoaderFS(const std::string& fs, ScriptReadMode mode) {
		// fragment shader.
		ShaderFS = glCreateShader(GL_FRAGMENT_SHADER);
		if (ShaderFS) {
			if (mode == StringFilepath)
				ShaderSrcFS = TextFileLoader(fs);
			else
				ShaderSrcFS = fs;
			const char* FssrcstrTemp = ShaderSrcFS.c_str();
			// read shader script_source => compile.
			glShaderSource(ShaderFS, SHADER_SCRIPT_COUNT, &FssrcstrTemp, NULL);
			glCompileShader(ShaderFS);

			if (!CompilationStatus(ShaderFS, "fragment"))
				ReturnResFlag = DEFRES_FLAG_INVALID;

			glAttachShader(ShaderProgram, ShaderFS);
		}
		else
			ImFXLog(LOG_ERROR, IMFX_OGLMAG_LABEL, "failed create fragment_shader handle.");
	}

	bool ImFXOGLshader::CreateCompileShaders() {
		// fragment,vertex shader =link=> program.
		glLinkProgram(ShaderProgram);
		return CompilationStatus(ShaderProgram, "program");
	}

	ImfxShader ImFXOGLshader::_MS_GETRES(ResourceFlag& flag) {
		flag = ReturnResFlag;
		return ShaderProgram;
	}

	/*
	* **************************************** Texture ****************************************
	* OpenGL 4.6 texture2d, stb_image image loader[.jpg.png]
	* Update: 2023.12.30 RCSZ.
	*/
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

	uint8_t* ImFXOGLtexture::ImageFileLoader(const std::string& file, int& width, int& height, int& channels) {
		uint8_t* ImageDataPointer = nullptr;

		if (std::filesystem::exists(file)) {
			// stbi_image loader.
			ImageDataPointer = stbi_load(file.c_str(), &width, &height, &channels, NULL);

			ImFXLog(LOG_INFO, IMFX_OGLMAG_LABEL, "loader: image file: %s", file.c_str());
			ImFXLog(LOG_INFO, IMFX_OGLMAG_LABEL, "loader: image size: %d x %d", width, height);
			ImFXLog(LOG_INFO, IMFX_OGLMAG_LABEL, "loader: image color_channel: %d", channels);
		}
		else
			ImFXLog(LOG_ERROR, IMFX_OGLMAG_LABEL, "image_loader failed open file: %s", file.c_str());
		return ImageDataPointer;
	}

#define GEN_TEXTURE_COUNT 1
	bool ImFXOGLtexture::CreateBindTexture(ImfxTexture& texture, GLenum type) {
		const char* TextureTag = {};
		if (type == GL_TEXTURE_2D) TextureTag = "2d";
		if (type == GL_TEXTURE_3D) TextureTag = "3d";

		// opengl generate texture handle.
		if (texture == NULL) {
			glGenTextures(GEN_TEXTURE_COUNT, &texture);
			glBindTexture(type, texture);

			// opengl handle index > 0.
			if (texture) { ImFXLog(LOG_INFO, IMFX_OGLMAG_LABEL, "texture.gen%s create.", TextureTag);           return DEF_IMFXSTAT_SUCCESS; }
			else         { ImFXLog(LOG_WARNING, IMFX_OGLMAG_LABEL, "texture.gen%s failed create.", TextureTag); return DEF_IMFXSTAT_FAILED;  }
		}
		else {
			// handle duplicate creation.
			ImFXLog(LOG_WARNING, IMFX_OGLMAG_LABEL, "texture.gen%s duplicate create.", TextureTag);
			return DEF_IMFXSTAT_FAILED;
		}
	}

	// nearest nighbor filtering.
	inline void FuncNearestNeighborFiltering(GLenum TYPE) {
		glTexParameteri(TYPE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(TYPE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	// linear filtering.
	inline void FuncLinearFiltering(GLenum TYPE) {
		glTexParameteri(TYPE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(TYPE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	// anisotropic filtering.
	inline void FuncAnisotropicFiltering(GLenum TYPE) {
		GLfloat MaxAnisotropy;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &MaxAnisotropy);
		glTexParameterf(TYPE, GL_TEXTURE_MAX_ANISOTROPY_EXT, MaxAnisotropy);
	}
	// generate mipmaps.
	inline void FuncGenerateMipmaps(GLenum TYPE) {
		glGenerateMipmap(TYPE);
	}

	void ConfigTextureFilter(TextureFilterMode filtering_flags, GLenum type) {
		// opengl texture2d config.
		if (filtering_flags & NearestNeighborFiltering) FuncNearestNeighborFiltering(type);
		if (filtering_flags & LinearFiltering)          FuncLinearFiltering(type);
		if (filtering_flags & AnisotropicFiltering)     FuncAnisotropicFiltering(type);
		if (filtering_flags & MipmapFiltering)          FuncGenerateMipmaps(type);
	}

	/*
	* constexpr float CLAMP_COLOR[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	* // texture clamp border.
	* glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	* glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	* // set clamp color.
	* glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, CLAMP_COLOR);
	*/

	inline void ConfigSurroundTex2D() {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	inline void ConfigSurroundTex3D() {
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	}

	bool ImFXOGLtexture::CreateTexture(uint32_t width, uint32_t height, uint32_t channels, uint8_t* data, TextureFilterMode mode) {
		if (data) {
			// [弃用项] GLU LIBRAY.
			// gluBuild2DMipmaps(GL_TEXTURE_2D, channels, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
			if (CreateBindTexture(TextureAttri.Texture, GL_TEXTURE_2D)) {

				// texture attribute config.
				TextureAttri.Channels = channels;
				TextureAttri.Width    = width;
				TextureAttri.Height   = height;

				// device_gpu tmu units number.
				TextureAttri.TextureSamplerCount = TextureSamplerUnit;

				if (channels == DEF_CHANNEL_RGB)
					glTexImage2D(GL_TEXTURE_2D, NULL, channels, width, height, NULL, GL_RGB, GL_UNSIGNED_BYTE, data);
				else if (channels == DEF_CHANNEL_RGBA)
					glTexImage2D(GL_TEXTURE_2D, NULL, channels, width, height, NULL, GL_RGBA, GL_UNSIGNED_BYTE, data);
				else
					ImFXLog(LOG_ERROR, IMFX_OGLMAG_LABEL, "texture image invalid channels.");

				// config texture2d. surround,filter.
				ConfigSurroundTex2D();
				ConfigTextureFilter(mode, GL_TEXTURE_2D);
				
				// unbind texture2d.
				glBindTexture(GL_TEXTURE_2D, NULL);
				ReturnResFlag = DEFRES_FLAG_NORMAL;
				return DEF_IMFXSTAT_SUCCESS;
			}
			return DEF_IMFXSTAT_FAILED;
		}
		else {
			ImFXLog(LOG_ERROR, IMFX_OGLMAG_LABEL, "texture data_ptr = nullptr.");
			return DEF_IMFXSTAT_FAILED;
		}
	}

	bool ImFXOGLtexture::CreateTextureEmpty(uint32_t width, uint32_t height, uint32_t channels, TextureFilterMode mode) {
		if (CreateBindTexture(TextureAttri.Texture, GL_TEXTURE_2D)) {

			// texture attribute config.
			TextureAttri.Channels = channels;
			TextureAttri.Width    = width;
			TextureAttri.Height   = height;

			// device_gpu tmu units number.
			TextureAttri.TextureSamplerCount = TextureSamplerUnit;

			if (channels == DEF_CHANNEL_RGB)
				glTexImage2D(GL_TEXTURE_2D, NULL, channels, width, height, NULL, GL_RGB, GL_FLOAT, nullptr);
			else if (channels == DEF_CHANNEL_RGBA)
				glTexImage2D(GL_TEXTURE_2D, NULL, channels, width, height, NULL, GL_RGBA, GL_FLOAT, nullptr);

			// config texture2d. surround,filter.
			ConfigSurroundTex2D();
			ConfigTextureFilter(mode, GL_TEXTURE_2D);

			// unbind texture2d.
			glBindTexture(GL_TEXTURE_2D, NULL);
			ReturnResFlag = DEFRES_FLAG_NORMAL;
			return DEF_IMFXSTAT_SUCCESS;
		}
		else
			return DEF_IMFXSTAT_FAILED;
	}

	bool ImFXOGLtexture::CreateTextureLoader(const std::string& file, TextureFilterMode mode) {
		int Width = NULL, Height = NULL, Channels = NULL;

		uint8_t* DataPtr = ImageFileLoader(file, Width, Height, Channels);
		return CreateTexture(Width, Height, Channels, DataPtr, mode);
	}

	TextureAttr ImFXOGLtexture::_MS_GETRES(ResourceFlag& flag) {
		flag = ReturnResFlag;
		return TextureAttri;
	}

	/*
	* **************************************** Model ****************************************
	* OpenGL 4.6 vertex_buffer,vertex_attribute static,dynamic
	* Update: 2023.12.30 RCSZ.
	*/

	void ImFXOGLmodel::VertexBufferSet(GLuint vao, GLuint vbo, size_t bytes, const float* verptr, uint32_t type) {
		// bind vao,vbo => data => unbind vao,vbo.
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBindVertexArray(vao);
		{
			switch (type) {
			case(0): { glBufferData(GL_ARRAY_BUFFER, bytes, verptr, GL_STATIC_DRAW); break;  }
			case(1): { glBufferData(GL_ARRAY_BUFFER, bytes, verptr, GL_DYNAMIC_DRAW); break; }
			}
		}
		glBindVertexArray(NULL);
		glBindBuffer(GL_ARRAY_BUFFER, NULL);
	}

	ImfxVertexAttribute ImFXOGLmodel::CreateVertexAttribute(uint32_t type, uint32_t begin_location) {
		uint32_t VertexAttributeHandle = NULL;

		// opengl generate vao handle.
		glGenVertexArrays(1, &VertexAttributeHandle);
		glBindVertexArray(VertexAttributeHandle);

		if (!VertexAttributeHandle)
			ImFXLog(LOG_ERROR, IMFX_OGLMAG_LABEL, "failed create vertex_attribute.");
		else {
			// "FS_VERTEX_BYTES" => "framework_define.hpp" (vertex bytes)
			switch (type) {
			case(0): { IMFX_OGL_VERATT::VERTEX_ATT_PRESET_1(FS_VERTEX_BYTES, begin_location); break;  }
			}
		}
		glBindVertexArray(NULL);

		return VertexAttributeHandle;
	}

	ImfxVertexBuffer ImFXOGLmodel::CreateVertexBuffer() {
		uint32_t VertexBufferHandle = NULL;

		// opengl generate vbo handle.
		glGenBuffers(1, &VertexBufferHandle);
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferHandle);

		if (!VertexBufferHandle)
			ImFXLog(LOG_ERROR, IMFX_OGLMAG_LABEL, "failed create vertex_buffer.");

		return VertexBufferHandle;
	}

	bool ImFXOGLmodel::CreateStaticModel(ImfxVertexAttribute veratt, ImfxVertexBuffer verbuf, const float* verptr, size_t bytes) {
		if (veratt && verbuf) {
			// static_model: write float vertex_data. (mode = 0)
			VertexBufferSet(veratt, verbuf, bytes, verptr, 0);

			// model attribute(vao,vbo) config.
			VerBufferAttri.DataAttribute     = veratt;
			VerBufferAttri.DataBuffer        = verbuf;
			VerBufferAttri.VertexBytes       = FS_VERTEX_BYTES;
			VerBufferAttri.VerticesDataBytes = bytes;

			ReturnResFlag = DEFRES_FLAG_NORMAL;
			return DEF_IMFXSTAT_SUCCESS;
		}
		else {
			ImFXLog(LOG_ERROR, IMFX_OGLMAG_LABEL, "static_model invalid handle(vao | vbo).");
			return DEF_IMFXSTAT_FAILED;
		}
	}

	bool ImFXOGLmodel::CreateDynamicModel(ImfxVertexAttribute veratt, ImfxVertexBuffer verbuf, const float* verptr, size_t bytes) {
		if (veratt && verbuf) {
			// dynamic_model: write float vertex_data. (mode = 1)
			VertexBufferSet(veratt, verbuf, bytes, verptr, 1);

			// model attribute(vao,vbo) config.
			VerBufferAttri.DataAttribute     = veratt;
			VerBufferAttri.DataBuffer        = verbuf;
			VerBufferAttri.VertexBytes       = FS_VERTEX_BYTES;
			VerBufferAttri.VerticesDataBytes = bytes;

			ReturnResFlag = DEFRES_FLAG_NORMAL;
			return DEF_IMFXSTAT_SUCCESS;
		}
		else {
			ImFXLog(LOG_ERROR, IMFX_OGLMAG_LABEL, "dynamic_model invalid handle(vao | vbo).");
			return DEF_IMFXSTAT_FAILED;
		}
	}

	VertexBufferAttr ImFXOGLmodel::_MS_GETRES(ResourceFlag& flag) {
		flag = ReturnResFlag;
		return VerBufferAttri;
	}

	/*
	* **************************************** FrameBuffer ****************************************
	* OpenGL 4.6 frame_buffer bind => texture, bind => render_buffer(x)
	* Update: 2023.12.30 RCSZ.
	*/

#define GEN_TEXTURE_COUNT 1
	bool ImFXOGLframebuffer::CreateBindFamebuffer(ImfxFrameBuffer& framebuf) {
		// opengl generate fbo handle.
		if (framebuf == NULL) {
			glGenFramebuffers(1, &framebuf);
			glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);

			// opengl handle index > 0.
			if (framebuf) { ImFXLog(LOG_INFO, IMFX_OGLMAG_LABEL, "framebuffer.gen create.");           return DEF_IMFXSTAT_SUCCESS; }
			else          { ImFXLog(LOG_WARNING, IMFX_OGLMAG_LABEL, "framebuffer.gen failed create."); return DEF_IMFXSTAT_FAILED;  }
		}
		else {
			// handle duplicate creation.
			ImFXLog(LOG_WARNING, IMFX_OGLMAG_LABEL, "framebuffer.gen duplicate create.");
			return DEF_IMFXSTAT_FAILED;
		}
	}

	bool ImFXOGLframebuffer::CreateBindTexture(const TextureAttr& texture) {
		if (CreateBindFamebuffer(FrameBuffer)) {
			// texture => framebuffer.
			glBindTexture(GL_TEXTURE_2D, texture.Texture);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.Texture, NULL);
			
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				ImFXLog(LOG_ERROR, IMFX_OGLMAG_LABEL, "incomplete framebuffer.");

			glBindFramebuffer(GL_FRAMEBUFFER, NULL);
			glBindTexture(GL_TEXTURE_2D, NULL);

			ReturnResFlag = DEFRES_FLAG_NORMAL;
			return DEF_IMFXSTAT_SUCCESS;
		}
		else
			return DEF_IMFXSTAT_FAILED;
	}

	// 暂时不启用(停工).
	bool ImFXOGLframebuffer::CreateBindRenderBuffer(ImfxRenderBuffer buffer) { return false; }

	ImfxFrameBuffer ImFXOGLframebuffer::_MS_GETRES(ResourceFlag& flag) {
		flag = ReturnResFlag;
		return FrameBuffer;
	}

	/*
	* **************************************** Uniform ****************************************
	* OpenGL 4.6 GLSL uniform variable.
	* Update: 2023.12.30 RCSZ.
	*/

	bool ImFxOGLuniform::ProgramHandle(GLuint program, const char* label) {
		if (program)
			return DEF_IMFXSTAT_SUCCESS;
		else {
			ImFXLog(LOG_WARNING, IMFX_OGLMAG_LABEL, "uniform_%s invalid shader_program.", label);
			return DEF_IMFXSTAT_FAILED;
		}
	}

	void ImFxOGLuniform::UniformMatrix3x3(ImfxShader program, const char* name, const FsMatrix3& matrix) {
		if (ProgramHandle(program, "mat3x3")) {
			int32_t uniformLocation = glGetUniformLocation(program, name);
			glUniformMatrix3fv(uniformLocation, 1, false, matrix.matrix);
		}
	}

	void ImFxOGLuniform::UniformMatrix4x4(ImfxShader program, const char* name, const FsMatrix4& matrix) {
		if (ProgramHandle(program, "mat4x4")) {
			int32_t uniformLocation = glGetUniformLocation(program, name);
			glUniformMatrix4fv(uniformLocation, 1, false, matrix.matrix);
		}
	}

	void ImFxOGLuniform::UniformInteger(ImfxShader program, const char* name, const int32_t& value) {
		if (ProgramHandle(program, "int")) {
			int32_t uniformLocation = glGetUniformLocation(program, name);
			glUniform1i(uniformLocation, value);
		}
	}

	void ImFxOGLuniform::UniformFloat(ImfxShader program, const char* name, const float& value) {
		if (ProgramHandle(program, "float")) {
			int32_t uniformLocation = glGetUniformLocation(program, name);
			glUniform1f(uniformLocation, value);
		}
	}

	void ImFxOGLuniform::UniformVec2(ImfxShader program, const char* name, const Vector2T<float>& value) {
		if (ProgramHandle(program, "vec2f")) {
			int32_t uniformLocation = glGetUniformLocation(program, name);
			glUniform2f(uniformLocation, value.vector_x, value.vector_y);
		}
	}

	void ImFxOGLuniform::UniformVec3(ImfxShader program, const char* name, const Vector3T<float>& value) {
		if (ProgramHandle(program, "vec3f")) {
			int32_t uniformLocation = glGetUniformLocation(program, name);
			glUniform3f(uniformLocation, value.vector_x, value.vector_y, value.vector_z);
		}
	}

	void ImFxOGLuniform::UniformVec4(ImfxShader program, const char* name, const Vector4T<float>& value) {
		if (ProgramHandle(program, "vec4f")) {
			int32_t uniformLocation = glGetUniformLocation(program, name);
			glUniform4f(uniformLocation, value.vector_x, value.vector_y, value.vector_z, value.vector_w);
		}
	}
}

namespace RenderingSupport {

	void RenderBindShader(const ImfxShader& program) { glUseProgram(program); }
	void RenderUnbindShader()                        { glUseProgram(NULL);    }

	void RenderBindTexture(const TextureAttr& texture) {
		glActiveTexture(GL_TEXTURE0 + texture.TextureSamplerCount);
		glBindTexture(GL_TEXTURE_2D, texture.Texture);
	}
	void RenderUnbindTexture() {
		glBindTexture(GL_TEXTURE_2D, NULL);
	}

	void RnenderFrameBufferA(const ImfxFrameBuffer& framebuffer) { glBindFramebuffer(GL_FRAMEBUFFER, framebuffer); }
	void RnenderFrameBufferB() {
		glBindFramebuffer(GL_FRAMEBUFFER, NULL);
		glBindFramebuffer(GL_FRAMEBUFFER, NULL);
	}

	void RenderDrawModel(const VertexBufferAttr& model) {
		glBindVertexArray(model.DataAttribute);
		glBindBuffer(GL_ARRAY_BUFFER, model.DataBuffer);
		glDrawArrays(GL_TRIANGLES, NULL, GLsizei(model.VerticesDataBytes / model.VertexBytes));
		glBindVertexArray(NULL);
		glBindBuffer(GL_ARRAY_BUFFER, NULL);
	}

	void UpdateModel(VertexBufferAttr* model, float* verptr, size_t bytes) {
		// update vertices dataset bytes_param.
		model->VerticesDataBytes = bytes;
		glBindBuffer(GL_ARRAY_BUFFER, model->DataBuffer);
		glBufferData(GL_ARRAY_BUFFER, bytes, verptr, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, NULL);
	}
}

/*
* **************************************** Information ****************************************
* OpenGL 4.6 get device info. (老版本图形底层移植过来)
* Update: 2023.12.30 RCSZ.
*/

NST_ImFxOGLinformation::NST_ImFxOGLinformation(const char* command) {
	// load hash_map(cmd_function).
	HashCmdFunc["Sysinfo PlatformGPU"]    = std::function<void(NST_ImFxOGLinformation*)>(&NST_ImFxOGLinformation::INFOgetGPUparameter);
	HashCmdFunc["Sysinfo OpenGLattribs"]  = std::function<void(NST_ImFxOGLinformation*)>(&NST_ImFxOGLinformation::INFOgetAttribsMax);
	HashCmdFunc["Sysinfo OpenGLtexture"]  = std::function<void(NST_ImFxOGLinformation*)>(&NST_ImFxOGLinformation::INFOgetTextureMax);
	HashCmdFunc["Sysinfo MemoryCapacity"] = std::function<void(NST_ImFxOGLinformation*)>(&NST_ImFxOGLinformation::INFOgetGPUmemory);
	HashCmdFunc["Sysinfo MemorySize"]     = std::function<void(NST_ImFxOGLinformation*)>(&NST_ImFxOGLinformation::INFOgetGPUmemoryUse);

	INFOcommand(command);
}

void NST_ImFxOGLinformation::INFOcommand(const char* _command) {
	if (HashCmdFunc.find(_command) != HashCmdFunc.end())
		HashCmdFunc[_command](this);
}

void NST_ImFxOGLinformation::INFOgetAttribsMax() {
	GLint AttributesMax = NULL;
	GLint BufferMax = NULL;

	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &AttributesMax);
	glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &BufferMax);

	TempValue = (size_t)AttributesMax;

	ImFXLog(LOG_INFO, IMFX_OGLMAG_LABEL, "device (vao)attribs_max units: %d entry", AttributesMax);
	ImFXLog(LOG_INFO, IMFX_OGLMAG_LABEL, "device (vbo)buffer_max units: %d entry", BufferMax);
}

void NST_ImFxOGLinformation::INFOgetTextureMax() {
	GLint TextureUnitsMax = NULL;
	// GL_MAX_TEXTURE_UNITS       - 基本纹理单元数量(可用于固定管线).
	// GL_MAX_TEXTURE_IMAGE_UNITS - 可以用于 Shader 的纹理单元数量.
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &TextureUnitsMax);
	TempValue = (size_t)TextureUnitsMax;
	ImFXLog(LOG_INFO, IMFX_OGLMAG_LABEL, "device [tmu]texture_max units: %d entry", TextureUnitsMax);
}

void NST_ImFxOGLinformation::INFOgetGPUmemory() {
	GLint MemoryKbytes = NULL;

	glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &MemoryKbytes);
	TempValue = (size_t)MemoryKbytes;
	ImFXLog(LOG_PERF, IMFX_OGLMAG_LABEL, "device memory[capacity]: %d mib", MemoryKbytes / 1024);
}

void NST_ImFxOGLinformation::INFOgetGPUmemoryUse() {
	GLint MemoryKbytes = NULL;

	glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &MemoryKbytes);
	TempValue = (size_t)MemoryKbytes;
	ImFXLog(LOG_PERF, IMFX_OGLMAG_LABEL, "device memory[size]: %d mib", MemoryKbytes / 1024);
}

void NST_ImFxOGLinformation::INFOgetGPUparameter() {
	ImFXLog(LOG_INFO, IMFX_OGLMAG_LABEL, "device gpu supplier: %s", (const char*)glGetString(GL_VENDOR));
	ImFXLog(LOG_INFO, IMFX_OGLMAG_LABEL, "device gpu model: %s", (const char*)glGetString(GL_RENDERER));
	ImFXLog(LOG_INFO, IMFX_OGLMAG_LABEL, "device version opengl: %s", (const char*)glGetString(GL_VERSION));
	ImFXLog(LOG_INFO, IMFX_OGLMAG_LABEL, "device version glsl: %s", (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
}

size_t NST_ImFxOGLinformation::GetValue() { return TempValue; };