// framework_renderer_ext. renderer extension interface.
/*
* ImProFX IMFXGL 图形抽象层扩展接口 Version: 0.1.0, Update: 2024.01.03
* 着色器支持: 顶点着色器, 片元着色器, 几何着色器, 曲面细分控制着色器, 曲面细分评估着色器, 计算着色器, 光线追踪着色器.
* 当前兼容图形库: OpenGL
* 扩展接口只是预留(暂不开发)
*/

#ifndef _FRAMEWORK_RENDERER_EXT_H
#define _FRAMEWORK_RENDERER_EXT_H

#define ENABLE_OPENGL_API
#include "framework_renderer.h"

// graphics pipeline.
class Ext_ImFxGLmanagerShaderGP {
public:
	// loader: vertex, fragment shader.
	virtual void ShaderLoaderVS(const std::string& vs, ScriptReadMode mode = StringFilepath) = 0;
	virtual void ShaderLoaderFS(const std::string& fs, ScriptReadMode mode = StringFilepath) = 0;

	// loader: geometry, tessellation_control, tessellation_evaluation shader.
	virtual void ShaderLoaderGES(const std::string& exts, ScriptReadMode mode = StringFilepath) = 0;
	virtual void ShaderLoaderTCS(const std::string& exts, ScriptReadMode mode = StringFilepath) = 0;
	virtual void ShaderLoaderTES(const std::string& exts, ScriptReadMode mode = StringFilepath) = 0;

	virtual bool CreateCompileShaders() = 0;

	virtual ImfxShader _MS_GETRES(ResourceFlag& flag) = 0;
};

// compute pipeline.
class Ext_ImFxGLmanagerShaderCP {
public:
	// loader: compute shader.
	virtual void ShaderLoaderCS(const std::string& exts, ScriptReadMode mode = StringFilepath) = 0;

	virtual bool CreateCompileShaders() = 0;

	virtual ImfxShader _MS_GETRES(ResourceFlag& flag) = 0;
};

// ray_tracing pipeline.
class Ext_ImFxGLmanagerShaderRTP {
public:
	// loader: ray_tracing shader.
	virtual void ShaderLoaderRTS(const std::string& exts, ScriptReadMode mode = StringFilepath) = 0;
	virtual bool DeviceRayTracing() = 0;

	virtual bool CreateCompileShaders() = 0;

	virtual ImfxShader _MS_GETRES(ResourceFlag& flag) = 0;
};

// IMFXGL EXT texture attribute. (texture3d)
struct Ext_TextureAttr3D {
	uint32_t Layers, Width, Height, Channels;
	ImfxTexture Texture;

	Ext_TextureAttr3D() : Texture{}, Layers(0), Width(0), Height(0), Channels(0) {}
	Ext_TextureAttr3D(ImfxTexture tex, uint32_t num, uint32_t x, uint32_t y, uint32_t ch) :
		Texture(tex), Layers(num), Width(x), Height(y), Channels(ch)
	{}
};

class Ext_ImFxGLmanagerTexture3D {
	virtual bool CreateTexture(
		uint32_t          layers, 
		uint32_t          width, 
		uint32_t          height, 
		uint32_t          channels, 
		uint8_t*          data, 
		TextureFilterMode mode
	) = 0;
	virtual bool CreateTextureEmpty(uint32_t layers, uint32_t width, uint32_t height, uint32_t channels, TextureFilterMode mode) = 0;
	virtual bool CreateTextureLoader(const std::vector<std::string>& files, TextureFilterMode mode) = 0;

	virtual Ext_TextureAttr3D _MS_GETRES(ResourceFlag& flag) = 0;
};

#endif