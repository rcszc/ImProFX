// framework_renderer_std. renderer standard interface.
/* 
* ImProFX IMFXGL 图形抽象层标准接口 Version: 0.1.7, Update: 2024.01.04
* 着色器支持: 顶点着色器, 片元着色器.
* 当前兼容图形库: OpenGL
* "Manager"抽象图形资源调用方法, "Resource"抽象图形资源存储[线程安全]
* 目前不具备兼容其他图形库的能力(后期会调整)
*/

#ifndef _FRAMEWORK_RENDERER_STD_H
#define _FRAMEWORK_RENDERER_STD_H
#include "framework_renderer.h"

// renderer layer initialization.
class IMFXGL_GLOBAL_INIT {
public:
	virtual INIT_RETURN RendererInit(INIT_PARAMETERS init_param) = 0;
	virtual void LoggerFunc(RendererLogger function) = 0;
};

// ******************************** Manager ********************************
// Renderer IMFXGL, FunctionManager.

class ImFxGLmanagerShader {
public:
	// loader: vertex, fragment shader.
	virtual void ShaderLoaderVS(const std::string& vs, ScriptReadMode mode = StringFilepath) = 0;
	virtual void ShaderLoaderFS(const std::string& fs, ScriptReadMode mode = StringFilepath) = 0;

	virtual bool CreateCompileShaders() = 0;

	virtual ImfxShader _MS_GETRES(ResourceFlag& flag) = 0;
};

// IMFXGL texture attribute. (texture2d)
struct TextureAttr {
	uint32_t Width, Height, Channels;
	uint32_t TextureSamplerCount;

	ImfxTexture Texture;

	TextureAttr() : Texture{}, Width(0), Height(0), Channels(0), TextureSamplerCount(0) {}
	TextureAttr(ImfxTexture tex, uint32_t x, uint32_t y, uint32_t ch, uint32_t tsc) :
		Texture(tex), Width(x), Height(y), Channels(ch), TextureSamplerCount(tsc)
	{}
};

class ImFxGLmanagerTexture {
public:
	virtual bool CreateTexture(uint32_t width, uint32_t height, uint32_t channels, uint8_t* data, TextureFilterMode mode) = 0;
	virtual bool CreateTextureEmpty(uint32_t width, uint32_t height, uint32_t channels, TextureFilterMode mode) = 0;
	virtual bool CreateTextureLoader(const std::string& file, TextureFilterMode mode) = 0;

	virtual TextureAttr _MS_GETRES(ResourceFlag& flag) = 0;
};

// IMFXGL vertex_buffer & vertex_attribute.
struct VertexBufferAttr {
	size_t VertexBytes, VerticesDataBytes;

	ImfxVertexBuffer DataBuffer;
	ImfxVertexAttribute DataAttribute;

	VertexBufferAttr() : DataBuffer{}, DataAttribute{}, VertexBytes(0), VerticesDataBytes(0) {}
	VertexBufferAttr(ImfxVertexBuffer vb, ImfxVertexAttribute va, size_t ver, size_t size) :
		DataBuffer(vb), DataAttribute(va), VertexBytes(ver), VerticesDataBytes(size)
	{}
};

class ImFxGLmanagerModel {
public:
	virtual ImfxVertexAttribute CreateVertexAttribute(uint32_t type, uint32_t begin_location = 0) = 0;
	virtual ImfxVertexBuffer CreateVertexBuffer() = 0;

	virtual bool CreateStaticModel(ImfxVertexAttribute veratt, ImfxVertexBuffer verbuf, const float* verptr, size_t bytes) = 0;
	virtual bool CreateDynamicModel(ImfxVertexAttribute veratt, ImfxVertexBuffer verbuf, const float* verptr, size_t bytes) = 0;

	virtual VertexBufferAttr _MS_GETRES(ResourceFlag& flag) = 0;
};

// IMFXGL render_buffer(rbo) attribute.
struct RenderBufferAttr {
	uint32_t Width, Height, Channels;
	size_t ImageBytes;

	ImfxRenderBuffer RenderBuffer;

	RenderBufferAttr() : RenderBuffer{}, Width(NULL), Height(NULL), Channels(NULL), ImageBytes(NULL) {}
	RenderBufferAttr(uint32_t x, uint32_t y, uint32_t ch, ImfxRenderBuffer rb) :
		Width(x), Height(y), Channels(ch), ImageBytes(size_t(x* y* ch) * sizeof(float)), RenderBuffer(rb)
	{}
};

class ImFxGLmanagerFrameBuffer {
public:
	virtual bool CreateBindTexture(const TextureAttr& texture) = 0;
	virtual bool CreateBindRenderBuffer(RenderBufferAttr buffer) = 0;

	virtual ImfxFrameBuffer _MS_GETRES(ResourceFlag& flag) = 0;
};

// IMFXGL render_buffer(rbo) get_data.
struct RenderBufferData {
	uint32_t Width, Height, Channels;
	std::vector<uint8_t> ImagePixels;
};

class ImFxGLmanagerRenderBuffer {
public:
	virtual bool CreateRenderBuffer(uint32_t width, uint32_t height) = 0;
	virtual RenderBufferData ReadRenderBuffer(RenderBufferAttr buffer) = 0;

	virtual RenderBufferAttr _MS_GETRES(ResourceFlag& flag) = 0;
};

class ImFxGLmangerUniform {
public:
	virtual void UniformMatrix3x3(ImfxShader program, const char* name, const FsMatrix3& matrix) = 0;
	virtual void UniformMatrix4x4(ImfxShader program, const char* name, const FsMatrix4& matrix) = 0;

	virtual void UniformFloat(ImfxShader program, const char* name, const float& value) = 0;
	virtual void UniformVec2(ImfxShader program, const char* name, const Vector2T<float>& value) = 0;
	virtual void UniformVec3(ImfxShader program, const char* name, const Vector3T<float>& value) = 0;
	virtual void UniformVec4(ImfxShader program, const char* name, const Vector4T<float>& value) = 0;
	virtual void UniformInteger(ImfxShader program, const char* name, const int32_t& value) = 0;
};

// ******************************** Resource(THREAD_SAFE) ********************************
// Renderer IMFXGL, ResourceProcessing.

// GPU resource: texture mapping unit.
class ImFxGLresourceTMU {
public:
	virtual uint32_t AllocTmuCount() = 0;
	virtual void FreeTmuCount(uint32_t count) = 0;
};
 
class ImFxGLresourceShader {
public:
	virtual ImfxShader ResourceFind(ResUnique key) = 0;
	virtual bool ResourceStorage(ResUnique key, ImFxGLmanagerShader* res) = 0;
	virtual bool ResourceDelete(ResUnique key) = 0;

	virtual size_t ResourceSize() = 0;
	virtual ~ImFxGLresourceShader() = default;
};

class ImFxGLresourceTexture {
public:
	virtual TextureAttr ResourceFind(ResUnique key) = 0;
	virtual bool ResourceStorage(ResUnique key, ImFxGLmanagerTexture* res) = 0;
	virtual bool ResourceDelete(ResUnique key) = 0;

	virtual size_t ResourceSize() = 0;
	virtual ~ImFxGLresourceTexture() = default;
};

// VertexBufferHD + VertexAttributeHD => VertexBufferAttr
class ImFxGLresourceVertexBuffer {
public:
	virtual VertexBufferAttr ResourceFind(ResUnique key) = 0;
	virtual bool ResourceStorage(ResUnique key, ImFxGLmanagerModel* res) = 0;
	virtual bool ResourceDelete(ResUnique key) = 0;

	virtual size_t ResourceSize() = 0;
	virtual ~ImFxGLresourceVertexBuffer() = default;
};

class ImFxGLresourceVertexAttribute {
public:
	virtual ImfxVertexAttribute ResourceFind(ResUnique key) = 0;
	virtual bool ResourceStorage(ResUnique key, ImfxVertexAttribute res) = 0;
	virtual bool ResourceDelete(ResUnique key) = 0;

	virtual size_t ResourceSize() = 0;
	virtual ~ImFxGLresourceVertexAttribute() = default;
};

class ImFxGLresourceFrameBuffer {
public:
	virtual ImfxFrameBuffer ResourceFind(ResUnique key) = 0;
	virtual bool ResourceStorage(ResUnique key, ImFxGLmanagerFrameBuffer* res) = 0;
	virtual bool ResourceDelete(ResUnique key) = 0;

	virtual size_t ResourceSize() = 0;
	virtual ~ImFxGLresourceFrameBuffer() = default;
};

class ImFxGLresourceRenderBuffer {
public:
	virtual RenderBufferAttr ResourceFind(ResUnique key) = 0;
	virtual bool ResourceStorage(ResUnique key, ImFxGLmanagerRenderBuffer* res) = 0;
	virtual bool ResourceDelete(ResUnique key) = 0;

	virtual size_t ResourceSize() = 0;
	virtual ~ImFxGLresourceRenderBuffer() = default;
};

#endif