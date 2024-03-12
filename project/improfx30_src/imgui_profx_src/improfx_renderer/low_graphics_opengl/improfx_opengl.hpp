// improfx_opengl. 2023.12.30 RCSZ.
// ImProFX Framework OpenGL LowLayer.

#ifndef _IMPROFX_OPENGL_H
#define _IMPROFX_OPENGL_H

#include "../framework_renderer_std.h"
#include "../framework_renderer_ext.h"

class ImFXOGLsystemLogger {
protected:
	static RendererLogger ImFXLog;
};

// Manager Module (Renderer"Manager"Interface)
// Update: 2024.01.25 RCSZ.
namespace IMFX_OGL_MAG {
#define IMFX_OGLMAG_LABEL "[IMFX_OGL_MAG]: "

	// system.persets data, 2 * triangle_att.
	constexpr float ShaderTemplateRect[72] = {
		-10.0f, -10.0f, 0.0f, 1.0f, 0.0f, 0.7f, 1.0f, 0.0f, 0.0f, 0.0f,0.0f,0.0f,
		 10.0f, -10.0f, 0.0f, 1.0f, 0.0f, 0.7f, 1.0f, 1.0f, 0.0f, 0.0f,0.0f,0.0f,
		 10.0f,  10.0f, 0.0f, 1.0f, 0.0f, 0.7f, 1.0f, 1.0f, 1.0f, 0.0f,0.0f,0.0f,

		-10.0f, -10.0f, 0.0f, 1.0f, 0.0f, 0.7f, 1.0f, 0.0f, 0.0f, 0.0f,0.0f,0.0f,
		 10.0f,  10.0f, 0.0f, 1.0f, 0.0f, 0.7f, 1.0f, 1.0f, 1.0f, 0.0f,0.0f,0.0f,
		-10.0f,  10.0f, 0.0f, 1.0f, 0.0f, 0.7f, 1.0f, 0.0f, 1.0f, 0.0f,0.0f,0.0f
	};
	constexpr size_t ShaderTemplateRectLen = 72;

	class ImFXOGLinit :public ImFXOGLsystemLogger, public IMFXGL_GLOBAL_INIT {
	public:
		// opengl initialization.
		INIT_RETURN RendererInit(INIT_PARAMETERS init_param = NULL);
		// framework logger function.
		void LoggerFunc(RendererLogger function);
	};

	class ImFXOGLshader :public ImFXOGLsystemLogger, public ImFxGLmanagerShader {
	private:
		std::string ShaderSrcVS = {}, ShaderSrcFS = {};
		GLuint ShaderVS = NULL, ShaderFS = NULL;

		ResourceFlag ReturnResFlag = DEFRES_FLAG_NORMAL;
		ImfxShader ShaderProgram = NULL;

		std::string TextFileLoader(const std::string& file);
		bool CompilationStatus(const uint32_t& shader, const std::string& label);

	public:
		ImFXOGLshader() : ShaderProgram(glCreateProgram()) {}
		// loader: vertex,fragment. script: [glsl]
		void ShaderLoaderVS(const std::string& vs, ScriptReadMode mode = StringFilepath) override;
		void ShaderLoaderFS(const std::string& fs, ScriptReadMode mode = StringFilepath) override;

		bool CreateCompileShaders() override;

		ImfxShader _MS_GETRES(ResourceFlag& flag) override;
	};

	class ImFXOGLtexture :public ImFXOGLsystemLogger, public ImFxGLmanagerTexture {
	private:
		ResourceFlag ReturnResFlag = DEFRES_FLAG_INVALID;
		TextureAttr TextureAttri = {};

		uint8_t* ImageFileLoader(const std::string& file, int& width, int& height, int& channels);
		bool CreateBindTexture(ImfxTexture& texture, GLenum type);

	public:
		bool CreateTexture(uint32_t width, uint32_t height, uint32_t channels, uint8_t* data, TextureFilterMode mode) override;
		bool CreateTextureEmpty(uint32_t width, uint32_t height, uint32_t channels, TextureFilterMode mode) override;
		bool CreateTextureLoader(const std::string& file, TextureFilterMode mode) override;

		uint32_t TextureSamplerUnit = 0;

		TextureAttr _MS_GETRES(ResourceFlag& flag) override;
	};

	class ImFXOGLmodel :public ImFXOGLsystemLogger, public ImFxGLmanagerModel {
	private:
		ResourceFlag ReturnResFlag = DEFRES_FLAG_INVALID;
		VertexBufferAttr VerBufferAttri = {};

		void VertexBufferSet(GLuint vao, GLuint vbo, size_t bytes, const float* verptr, uint32_t type);

	public:
		ImfxVertexAttribute CreateVertexAttribute(uint32_t type, uint32_t begin_location = 0) override;
		ImfxVertexBuffer CreateVertexBuffer() override;

		bool CreateStaticModel(ImfxVertexAttribute veratt, ImfxVertexBuffer verbuf, const float* verptr, size_t bytes) override;
		bool CreateDynamicModel(ImfxVertexAttribute veratt, ImfxVertexBuffer verbuf, const float* verptr, size_t bytes) override;

		VertexBufferAttr _MS_GETRES(ResourceFlag& flag) override;
	};

	class ImFXOGLrenderbuffer :public ImFXOGLsystemLogger, public ImFxGLmanagerRenderBuffer {
	private:
		ResourceFlag ReturnResFlag = DEFRES_FLAG_INVALID;
		RenderBufferAttr RenderBuffer = {};

		bool CreateBindRenderbuffer(ImfxRenderBuffer& renderbuf);

	public:
		bool CreateRenderBuffer(uint32_t width, uint32_t height);
		ImageRawData ReadRenderBuffer(RenderBufferAttr buffer);

		RenderBufferAttr _MS_GETRES(ResourceFlag& flag);
	};

	class ImFXOGLframebuffer :public ImFXOGLsystemLogger, public ImFxGLmanagerFrameBuffer {
	private:
		ResourceFlag ReturnResFlag = DEFRES_FLAG_INVALID;
		ImfxFrameBuffer FrameBuffer = NULL;

		bool CreateBindFamebuffer(ImfxFrameBuffer& framebuf);
		bool CheckFramebuffer(ImfxFrameBuffer& framebuf, ResourceFlag& flag);

	public:
		bool CreateBindTexture(const TextureAttr& texture) override;
		bool CreateBindRenderBuffer(RenderBufferAttr buffer) override;

		ImfxFrameBuffer _MS_GETRES(ResourceFlag& flag) override;
	};

	class ImFxOGLuniform :public ImFXOGLsystemLogger, public ImFxGLmanagerUniform {
	private:
		bool ProgramHandle(GLuint program, const char* label);
	public:
		void UniformMatrix3x3(ImfxShader program, const char* name, const FsMatrix3& matrix) override;
		void UniformMatrix4x4(ImfxShader program, const char* name, const FsMatrix4& matrix) override;

		void UniformInteger(ImfxShader program, const char* name, const int32_t& value) override;
		void UniformFloat(ImfxShader program, const char* name, const float& value) override;

		void UniformVec2(ImfxShader program, const char* name, const Vector2T<float>& value) override;
		void UniformVec3(ImfxShader program, const char* name, const Vector3T<float>& value) override;
		void UniformVec4(ImfxShader program, const char* name, const Vector4T<float>& value) override;
	};
}

// non_std: imfx_renderer interface.
// 2023.12.31 (OpenGL渲染支持函数).
namespace RenderingSupport {

	void RenderBindShader(const ImfxShader& program);
	void RenderUnbindShader();

	void RenderBindTexture(const TextureAttr& texture);
	void RenderUnbindTexture();

	void RnenderBindFrameBuffer(const ImfxFrameBuffer& framebuffer);
	void RnenderUnbindFrameBuffer();

	void RenderDrawModel(const VertexBufferAttr& model);
	// 配合'LLRES'VBO持久映射.
	void UpdateModel(VertexBufferAttr* model, float* verptr, size_t bytes);
}

// non_std: imfx_renderer interface.
class NST_ImFxOGLinformation :public ImFXOGLsystemLogger {
public:
	bool LoggerPrintInformation = false;

	// @return x:vao_max, y:vbo_max
	Vector2T<int32_t> InfoGetVertexUnitsMax();
	// @return device tmu_max
	int32_t InfoGetTextureUnitsMax();
	// @return device gpu memory_capacity [mib]
	float InfoGetGPUmemoryCapacity();
	// @return device gpu memory_usage [mib]
	float InfoGetGPUmemoryUsage();

	// logger print device info
	void InfoPrintPlatformParameters();
};

// Resource Module (Renderer"Resource"Interface) Thread-Safe.
// Update: 2024.01.02 RCSZ.
namespace IMFX_OGL_RES {
#define IMFX_OGLRES_LABEL "[IMFX_OGL_RES]: "

	class ImFXOGLresTexSampler :public ImFXOGLsystemLogger, public ImFxGLresourceTMU {
	protected:
		std::vector<bool> TmuStateFlag  = {};
		std::mutex TmuStateMutex = {};

	public:
		ImFXOGLresTexSampler(size_t tmu_size) {
			std::lock_guard<std::mutex> Lock(TmuStateMutex);
			TmuStateFlag.resize(tmu_size);
		}

		uint32_t AllocTmuCount() override;
		void FreeTmuCount(uint32_t count) override;
	};

	class ImFXOGLresShader :public ImFXOGLsystemLogger, public ImFxGLresourceShader {
	protected:
		std::unordered_map<std::string, ImfxShader> ResourceShaderMap   = {};
		std::mutex ResourceShaderMutex = {};

	public:
		ImfxShader ResourceFind(ResUnique key) override;
		bool ResourceStorage(ResUnique key, ImFxGLmanagerShader* res) override;
		bool ResourceDelete(ResUnique key) override;

		size_t ResourceSize() override {
			std::lock_guard<std::mutex> Lock(ResourceShaderMutex);
			return ResourceShaderMap.size();
		}

		~ImFXOGLresShader() override {
			uint32_t InvalidHDcount = NULL;
			// clear hashmap data(shader).
			for (const auto& item : ResourceShaderMap) {
				// 删除无效句柄无害(OGLAPI)
				glDeleteProgram(item.second);
				if (item.second == 0) ++InvalidHDcount;
			}
			// outlog: free_count, invalid_count.
			ImFXLog(LOG_TRACE, IMFX_OGLRES_LABEL, "free resource(shader): %u items", ResourceSize());
			if (InvalidHDcount)
				ImFXLog(LOG_WARNING, IMFX_OGLRES_LABEL, "invalid resource(shader): %u items", InvalidHDcount);
		}
	};

	class ImFXOGLresTexture :public ImFXOGLsystemLogger, public ImFxGLresourceTexture {
	protected:
		std::unordered_map<std::string, TextureAttr> ResourceTextureMap = {};
		std::mutex ResourceTextureMutex = {};

	public:
		TextureAttr ResourceFind(ResUnique key) override;
		bool ResourceStorage(ResUnique key, ImFxGLmanagerTexture* res) override;
		bool ResourceDelete(ResUnique key) override;

		size_t ResourceSize() override {
			std::lock_guard<std::mutex> Lock(ResourceTextureMutex);
			return ResourceTextureMap.size();
		}

		~ImFXOGLresTexture() override {
			uint32_t InvalidHDcount = NULL;
			// clear hashmap data(texture).
			for (const auto& item : ResourceTextureMap) {

				glDeleteTextures(1, &item.second.Texture);
				if (item.second.Texture == 0) ++InvalidHDcount;
			}
			// outlog: free_count, invalid_count.
			ImFXLog(LOG_TRACE, IMFX_OGLRES_LABEL, "free resource(texture): %u items", ResourceSize());
			if (InvalidHDcount)
				ImFXLog(LOG_WARNING, IMFX_OGLRES_LABEL, "invalid resource(texture): %u items", InvalidHDcount);
		}
	};

	class ImFXOGLresVertexBuffer :public ImFXOGLsystemLogger, public ImFxGLresourceVertexBuffer {
	protected:
		std::unordered_map<std::string, VertexBufferAttr> ResourceVertexBufferMap = {};
		std::mutex ResourceVertexBufferMutex = {};

	public:
		VertexBufferAttr ResourceFind(ResUnique key) override;
		bool ResourceStorage(ResUnique key, ImFxGLmanagerModel* res) override;
		bool ResourceDelete(ResUnique key) override;

		// VBO资源持久映射(用于动态更新,非标准接口) update: 2023.12.31
		VertexBufferAttr* ExtResourceMapping(ResUnique key);

		size_t ResourceSize() override {
			std::lock_guard<std::mutex> Lock(ResourceVertexBufferMutex);
			return ResourceVertexBufferMap.size();
		}

		~ImFXOGLresVertexBuffer() override {
			uint32_t InvalidHDcount = NULL;
			// clear hashmap data(vertex_buffer).
			for (const auto& item : ResourceVertexBufferMap) {

				glDeleteBuffers(1, &item.second.DataBuffer);
				if (item.second.DataBuffer == 0) ++InvalidHDcount;
			}
			// outlog: free_count, invalid_count.
			ImFXLog(LOG_TRACE, IMFX_OGLRES_LABEL, "free resource(vertex_buffer): %u items", ResourceSize());
			if (InvalidHDcount)
				ImFXLog(LOG_WARNING, IMFX_OGLRES_LABEL, "invalid resource(vertex_buffer): %u items", InvalidHDcount);
		}
	};

	class ImFXOGLresVertexAttr :public ImFXOGLsystemLogger, public ImFxGLresourceVertexAttribute {
	protected:
		std::unordered_map<std::string, ImfxVertexAttribute> ResourceVertexAttrMap = {};
		std::mutex ResourceVertexAttrMutex = {};

	public:
		ImfxVertexAttribute ResourceFind(ResUnique key) override;
		bool ResourceStorage(ResUnique key, ImfxVertexAttribute res) override;
		bool ResourceDelete(ResUnique key) override;

		size_t ResourceSize() override {
			std::lock_guard<std::mutex> Lock(ResourceVertexAttrMutex);
			return ResourceVertexAttrMap.size();
		}

		~ImFXOGLresVertexAttr() override {
			uint32_t InvalidHDcount = NULL;
			// clear hashmap data(vertex_attribute).
			for (const auto& item : ResourceVertexAttrMap) {

				glDeleteVertexArrays(1, &item.second);
				if (item.second == 0) ++InvalidHDcount;
			}
			// outlog: free_count, invalid_count.
			ImFXLog(LOG_TRACE, IMFX_OGLRES_LABEL, "free resource(vertex_attribute): %u items", ResourceSize());
			if (InvalidHDcount)
				ImFXLog(LOG_WARNING, IMFX_OGLRES_LABEL, "invalid resource(vertex_attribute): %u items", InvalidHDcount);
		}
	};

	class ImFXOGLresFrameBuffer :public ImFXOGLsystemLogger, public ImFxGLresourceFrameBuffer {
	protected:
		std::unordered_map<std::string, ImfxFrameBuffer> ResourceFrameBufferMap = {};
		std::mutex ResourceFrameBufferMutex = {};

	public:
		ImfxFrameBuffer ResourceFind(ResUnique key) override;
		bool ResourceStorage(ResUnique key, ImFxGLmanagerFrameBuffer* res) override;
		bool ResourceDelete(ResUnique key) override;

		size_t ResourceSize() override {
			std::lock_guard<std::mutex> Lock(ResourceFrameBufferMutex);
			return ResourceFrameBufferMap.size();
		}

		~ImFXOGLresFrameBuffer() override {
			uint32_t InvalidHDcount = NULL;
			// clear hashmap data(frame_buffer).
			for (const auto& item : ResourceFrameBufferMap) {

				glDeleteFramebuffers(1, &item.second);
				if (item.second == 0) ++InvalidHDcount;
			}
			// outlog: free_count, invalid_count.
			ImFXLog(LOG_TRACE, IMFX_OGLRES_LABEL, "free resource(frame_buffer): %u items", ResourceSize());
			if (InvalidHDcount)
				ImFXLog(LOG_WARNING, IMFX_OGLRES_LABEL, "invalid resource(frame_buffer): %u items", InvalidHDcount);
		}
	};

	class ImFXOGLresRenderBuffer :public ImFXOGLsystemLogger, public ImFxGLresourceRenderBuffer {
	protected:
		std::unordered_map<std::string, RenderBufferAttr> ResourceRenderBufferMap = {};
		std::mutex ResourceRenderBufferMutex = {};

	public:
		RenderBufferAttr ResourceFind(ResUnique key) override;
		bool ResourceStorage(ResUnique key, ImFxGLmanagerRenderBuffer* res) override;
		bool ResourceDelete(ResUnique key) override;

		size_t ResourceSize() override {
			std::lock_guard<std::mutex> Lock(ResourceRenderBufferMutex);
			return ResourceRenderBufferMap.size();
		}

		~ImFXOGLresRenderBuffer() override {
			uint32_t InvalidHDcount = NULL;
			// clear hashmap data(render_buffer).
			for (const auto& item : ResourceRenderBufferMap) {

				glDeleteRenderbuffers(1, &item.second.RenderBuffer);
				if (item.second.RenderBuffer == 0) ++InvalidHDcount;
			}
			// outlog: free_count, invalid_count.
			ImFXLog(LOG_TRACE, IMFX_OGLRES_LABEL, "free resource(render_buffer): %u items", ResourceSize());
			if (InvalidHDcount)
				ImFXLog(LOG_WARNING, IMFX_OGLRES_LABEL, "invalid resource(render_buffer): %u items", InvalidHDcount);
		}
	};
}

// Input/Output Module (Renderer"IO"Interface).
// Update: 2024.02.03 RCSZ.
namespace IMFX_OGL_IOP {
#define IMFX_OGLIOP_LABEL "[IMFX_OGL_IOP]: "

	class ImFXOGLioImage :public ImFXOGLsystemLogger, public ImFxIOmanagerImage {
	public:
		bool WriteImageFile(ImageRawData rawdata, std::string file, SaveImageMode mode = ImageJPG, float quality = 1.0f) override;
		ImageRawData ReadImageFile(std::string file) override;
	};
}

#endif