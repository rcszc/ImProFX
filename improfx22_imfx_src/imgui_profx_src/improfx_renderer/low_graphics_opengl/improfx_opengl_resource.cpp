// improfx_opengl_resource.
#include "improfx_opengl.hpp"

namespace IMFX_OGL_RES {

	inline bool FindTmuStateflag(std::vector<bool>& data, uint32_t& count) {
		bool IDLEresources = false;
		for (size_t i = 0; i < data.size(); ++i) {
			if (data[i] == false) {
				data[i] = true;
				IDLEresources = true;
				count = (uint32_t)i;
				break;
			}
		}
		return IDLEresources;
	}

	uint32_t ImFXOGLresTexSampler::AllocTmuCount() {
		uint32_t ReturnTmuCount = NULL;

		bool FindFlag = FindTmuStateflag(TmuStateFlag, ReturnTmuCount);
		if (!FindFlag)
			ImFXLog(LOG_WARNING, IMFX_OGLMAG_LABEL, "tmu failed alloc_count idle = 0.");
		else
			ImFXLog(LOG_TRACE, IMFX_OGLMAG_LABEL, "tmu alloc_count: %u", ReturnTmuCount);
		return ReturnTmuCount;
	}

	void ImFXOGLresTexSampler::FreeTmuCount(uint32_t count) {
		if (count >= TmuStateFlag.size())
			ImFXLog(LOG_ERROR, IMFX_OGLMAG_LABEL, "tmu failed recovery_count fc >= max.");
		else {
			TmuStateFlag[(size_t)count] = false;
			ImFXLog(LOG_TRACE, IMFX_OGLMAG_LABEL, "tmu recovery_count: %u", count);
		}
	}

	ImfxShader ImFXOGLresShader::ResourceFind(ResUnique key) {
		return (ResourceShaderMap.find(key) != ResourceShaderMap.end()) ? ResourceShaderMap[key] : NULL;
	}

	bool ImFXOGLresShader::ResourceStorage(ResUnique key, ImFxGLmanagerShader* res) {
		auto it = ResourceShaderMap.find(key);
		if (it != ResourceShaderMap.end()) {
			ImFXLog(LOG_WARNING, IMFX_OGLMAG_LABEL, "shader: failed storage duplicate_key: %s", key.c_str());
			return false;
		}
		else {
			ResourceFlag ResFlag = {};
			auto ResStorage = res->_MS_GETRES(ResFlag);
			// resource = normal ?
			if (ResFlag == DEFRES_FLAG_NORMAL) {
				ResourceShaderMap[key] = ResStorage;
				ImFXLog(LOG_INFO, IMFX_OGLMAG_LABEL, "shader: storage key: %s", key.c_str());
				return true;
			}
			else {
				ImFXLog(LOG_ERROR, IMFX_OGLMAG_LABEL, "shader: failed storage res_err key: %s, code: %i", key.c_str(), ResFlag);
				return false;
			}
		}
	}

	bool ImFXOGLresShader::ResourceDelete(ResUnique key) {
		auto it = ResourceShaderMap.find(key);
		if (it != ResourceShaderMap.end()) {

			// delete shader program => clear map_item.
			glDeleteProgram(it->second);
			ResourceShaderMap.erase(it);

			ImFXLog(LOG_INFO, IMFX_OGLMAG_LABEL, "shader: delete key: %s", key.c_str());
			return true;
		}
		else {
			ImFXLog(LOG_WARNING, IMFX_OGLMAG_LABEL, "shader: failed delete, not found key.");
			return false;
		}
	}

	TextureAttr ImFXOGLresTexture::ResourceFind(ResUnique key) {
		return (ResourceTextureMap.find(key) != ResourceTextureMap.end()) ? ResourceTextureMap[key] : TextureAttr();
	}

	bool ImFXOGLresTexture::ResourceStorage(ResUnique key, ImFxGLmanagerTexture* res) {
		auto it = ResourceTextureMap.find(key);
		if (it != ResourceTextureMap.end()) {
			ImFXLog(LOG_WARNING, IMFX_OGLMAG_LABEL, "texture: failed storage duplicate_key: %s", key.c_str());
			return false;
		}
		else {
			ResourceFlag ResFlag = {};
			auto ResStorage = res->_MS_GETRES(ResFlag);
			// resource = normal ?
			if (ResFlag == DEFRES_FLAG_NORMAL) {
				ResourceTextureMap[key] = ResStorage;
				ImFXLog(LOG_INFO, IMFX_OGLMAG_LABEL, "texture: storage key: %s", key.c_str());
				return true;
			}
			else {
				ImFXLog(LOG_ERROR, IMFX_OGLMAG_LABEL, "texture: failed storage res_err key: %s, code: %i", key.c_str(), ResFlag);
				return false;
			}
		}
	}

	bool ImFXOGLresTexture::ResourceDelete(ResUnique key) {
		auto it = ResourceTextureMap.find(key);
		if (it != ResourceTextureMap.end()) {

			// delete texture handle => clear map_item.
			glDeleteTextures(1, &it->second.Texture);
			ResourceTextureMap.erase(it);

			ImFXLog(LOG_INFO, IMFX_OGLMAG_LABEL, "texture: delete key: %s", key.c_str());
			return true;
		}
		else {
			ImFXLog(LOG_WARNING, IMFX_OGLMAG_LABEL, "texture: failed delete, not found key.");
			return false;
		}
	}

	VertexBufferAttr ImFXOGLresVertexBuffer::ResourceFind(ResUnique key) {
		return (ResourceVertexBufferMap.find(key) != ResourceVertexBufferMap.end()) ? ResourceVertexBufferMap[key] : VertexBufferAttr();
	}

	bool ImFXOGLresVertexBuffer::ResourceStorage(ResUnique key, ImFxGLmanagerModel* res) {
		auto it = ResourceVertexBufferMap.find(key);
		if (it != ResourceVertexBufferMap.end()) {
			ImFXLog(LOG_WARNING, IMFX_OGLMAG_LABEL, "verbuffer: failed storage duplicate_key: %s", key.c_str());
			return false;
		}
		else {
			ResourceFlag ResFlag = {};
			auto ResStorage = res->_MS_GETRES(ResFlag);
			// resource = normal ?
			if (ResFlag == DEFRES_FLAG_NORMAL) {
				ResourceVertexBufferMap[key] = ResStorage;
				ImFXLog(LOG_INFO, IMFX_OGLMAG_LABEL, "verbuffer: storage key: %s", key.c_str());
				return true;
			}
			else {
				ImFXLog(LOG_ERROR, IMFX_OGLMAG_LABEL, "verbuffer: failed storage res_err key: %s, code: %i", key.c_str(), ResFlag);
				return false;
			}
		}
	}

	bool ImFXOGLresVertexBuffer::ResourceDelete(ResUnique key) {
		auto it = ResourceVertexBufferMap.find(key);
		if (it != ResourceVertexBufferMap.end()) {

			// delete vbo handle => clear map_item. (VAO只为映射所以不由此管理)
			glDeleteBuffers(1, &it->second.DataBuffer);
			ResourceVertexBufferMap.erase(it);

			ImFXLog(LOG_INFO, IMFX_OGLMAG_LABEL, "verbuffer: delete key: %s", key.c_str());
			return true;
		}
		else {
			ImFXLog(LOG_WARNING, IMFX_OGLMAG_LABEL, "verbuffer: failed delete, not found key.");
			return false;
		}
	}

	VertexBufferAttr* ImFXOGLresVertexBuffer::ExtResourceMapping(ResUnique key) {
		return (ResourceVertexBufferMap.find(key) != ResourceVertexBufferMap.end()) ? &ResourceVertexBufferMap[key] : nullptr;
	}

	ImfxVertexAttribute ImFXOGLresVertexAttr::ResourceFind(ResUnique key) {
		return (ResourceVertexAttrMap.find(key) != ResourceVertexAttrMap.end()) ? ResourceVertexAttrMap[key] : NULL;
	}

	bool ImFXOGLresVertexAttr::ResourceStorage(ResUnique key, ImfxVertexAttribute res) {
		auto it = ResourceVertexAttrMap.find(key);
		if (it != ResourceVertexAttrMap.end()) {
			ImFXLog(LOG_WARNING, IMFX_OGLMAG_LABEL, "veratt: failed storage duplicate_key: %s", key.c_str());
			return false;
		}
		else {
			ResourceVertexAttrMap[key] = res;
			ImFXLog(LOG_INFO, IMFX_OGLMAG_LABEL, "veratt: storage key: %s", key.c_str());
			return true;
		}
	}

	bool ImFXOGLresVertexAttr::ResourceDelete(ResUnique key) {
		auto it = ResourceVertexAttrMap.find(key);
		if (it != ResourceVertexAttrMap.end()) {

			// delete vao handle => clear map_item.
			glDeleteVertexArrays(1, &it->second);
			ResourceVertexAttrMap.erase(it);

			ImFXLog(LOG_INFO, IMFX_OGLMAG_LABEL, "veratt: delete key: %s", key.c_str());
			return true;
		}
		else {
			ImFXLog(LOG_WARNING, IMFX_OGLMAG_LABEL, "veratt: failed delete, not found key.");
			return false;
		}
	}

	ImfxFrameBuffer ImFXOGLresFrameBuffer::ResourceFind(ResUnique key) {
		return (ResourceFrameBufferMap.find(key) != ResourceFrameBufferMap.end()) ? ResourceFrameBufferMap[key] : NULL;
	}

	bool ImFXOGLresFrameBuffer::ResourceStorage(ResUnique key, ImFxGLmanagerFrameBuffer* res) {
		auto it = ResourceFrameBufferMap.find(key);
		if (it != ResourceFrameBufferMap.end()) {
			ImFXLog(LOG_WARNING, IMFX_OGLMAG_LABEL, "framebuf: failed storage duplicate_key: %s", key.c_str());
			return false;
		}
		else {
			ResourceFlag ResFlag = {};
			auto ResStorage = res->_MS_GETRES(ResFlag);
			// resource = normal ?
			if (ResFlag == DEFRES_FLAG_NORMAL) {
				ResourceFrameBufferMap[key] = ResStorage;
				ImFXLog(LOG_INFO, IMFX_OGLMAG_LABEL, "framebuf: storage key: %s", key.c_str());
				return true;
			}
			else {
				ImFXLog(LOG_ERROR, IMFX_OGLMAG_LABEL, "framebuf: failed storage res_err key: %s, code: %i", key.c_str(), ResFlag);
				return false;
			}
		}
	}

	bool ImFXOGLresFrameBuffer::ResourceDelete(ResUnique key) {
		auto it = ResourceFrameBufferMap.find(key);
		if (it != ResourceFrameBufferMap.end()) {

			// delete fbo handle => clear map_item.
			glDeleteFramebuffers(1, &it->second);
			ResourceFrameBufferMap.erase(it);

			ImFXLog(LOG_INFO, IMFX_OGLMAG_LABEL, "framebuf: delete key: %s", key.c_str());
			return true;
		}
		else {
			ImFXLog(LOG_WARNING, IMFX_OGLMAG_LABEL, "framebuf: failed delete, not found key.");
			return false;
		}
	}
}