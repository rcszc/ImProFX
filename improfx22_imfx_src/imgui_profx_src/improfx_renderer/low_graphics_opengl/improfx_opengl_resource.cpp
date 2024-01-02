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
		bool FindFlag = false;
		uint32_t ReturnTmuCount = NULL;
		{
			std::lock_guard<std::mutex> Lock(TmuStateMutex);
			bool FindFlag = FindTmuStateflag(TmuStateFlag, ReturnTmuCount);
		}
		if (FindFlag)
			ImFXLog(LOG_WARNING, IMFX_OGLRES_LABEL, "tmu failed alloc_count idle = 0.");
		else
			ImFXLog(LOG_TRACE, IMFX_OGLRES_LABEL, "tmu alloc_count: %u", ReturnTmuCount);
		return ReturnTmuCount;
	}

	void ImFXOGLresTexSampler::FreeTmuCount(uint32_t count) {
		std::lock_guard<std::mutex> Lock(TmuStateMutex);

		if (count >= TmuStateFlag.size())
			ImFXLog(LOG_ERROR, IMFX_OGLRES_LABEL, "tmu failed recovery_count fc >= max.");
		else {
			TmuStateFlag[(size_t)count] = false;
			ImFXLog(LOG_TRACE, IMFX_OGLRES_LABEL, "tmu recovery_count: %u", count);
		}
	}

	ImfxShader ImFXOGLresShader::ResourceFind(ResUnique key) {
		std::lock_guard<std::mutex> Lock(ResourceShaderMutex);
		return (ResourceShaderMap.find(key) != ResourceShaderMap.end()) ? ResourceShaderMap[key] : NULL;
	}

	bool ImFXOGLresShader::ResourceStorage(ResUnique key, ImFxGLmanagerShader* res) {
		std::lock_guard<std::mutex> Lock(ResourceShaderMutex);

		auto it = ResourceShaderMap.find(key);
		if (it != ResourceShaderMap.end()) {
			ImFXLog(LOG_WARNING, IMFX_OGLRES_LABEL, "shader: failed storage duplicate_key: %s", key.c_str());
			return false;
		}
		else {
			ResourceFlag ResFlag = {};
			auto ResStorage = res->_MS_GETRES(ResFlag);
			// resource = normal ?
			if (ResFlag == DEFRES_FLAG_NORMAL) {
				ResourceShaderMap[key] = ResStorage;
				ImFXLog(LOG_INFO, IMFX_OGLRES_LABEL, "shader: storage key: %s", key.c_str());
				return true;
			}
			else {
				ImFXLog(LOG_ERROR, IMFX_OGLRES_LABEL, "shader: failed storage res_err key: %s, code: %i", key.c_str(), ResFlag);
				return false;
			}
		}
	}

	bool ImFXOGLresShader::ResourceDelete(ResUnique key) {
		std::lock_guard<std::mutex> Lock(ResourceShaderMutex);

		auto it = ResourceShaderMap.find(key);
		if (it != ResourceShaderMap.end()) {

			// delete shader program => clear map_item.
			glDeleteProgram(it->second);
			ResourceShaderMap.erase(it);

			ImFXLog(LOG_INFO, IMFX_OGLRES_LABEL, "shader: delete key: %s", key.c_str());
			return true;
		}
		else {
			ImFXLog(LOG_WARNING, IMFX_OGLRES_LABEL, "shader: failed delete, not found key.");
			return false;
		}
	}

	TextureAttr ImFXOGLresTexture::ResourceFind(ResUnique key) {
		std::lock_guard<std::mutex> Lock(ResourceTextureMutex);
		return (ResourceTextureMap.find(key) != ResourceTextureMap.end()) ? ResourceTextureMap[key] : TextureAttr();
	}

	bool ImFXOGLresTexture::ResourceStorage(ResUnique key, ImFxGLmanagerTexture* res) {
		std::lock_guard<std::mutex> Lock(ResourceTextureMutex);

		auto it = ResourceTextureMap.find(key);
		if (it != ResourceTextureMap.end()) {
			ImFXLog(LOG_WARNING, IMFX_OGLRES_LABEL, "texture: failed storage duplicate_key: %s", key.c_str());
			return false;
		}
		else {
			ResourceFlag ResFlag = {};
			auto ResStorage = res->_MS_GETRES(ResFlag);
			// resource = normal ?
			if (ResFlag == DEFRES_FLAG_NORMAL) {
				ResourceTextureMap[key] = ResStorage;
				ImFXLog(LOG_INFO, IMFX_OGLRES_LABEL, "texture: storage key: %s", key.c_str());
				return true;
			}
			else {
				ImFXLog(LOG_ERROR, IMFX_OGLRES_LABEL, "texture: failed storage res_err key: %s, code: %i", key.c_str(), ResFlag);
				return false;
			}
		}
	}

	bool ImFXOGLresTexture::ResourceDelete(ResUnique key) {
		std::lock_guard<std::mutex> Lock(ResourceTextureMutex);

		auto it = ResourceTextureMap.find(key);
		if (it != ResourceTextureMap.end()) {

			// delete texture handle => clear map_item.
			glDeleteTextures(1, &it->second.Texture);
			ResourceTextureMap.erase(it);

			ImFXLog(LOG_INFO, IMFX_OGLRES_LABEL, "texture: delete key: %s", key.c_str());
			return true;
		}
		else {
			ImFXLog(LOG_WARNING, IMFX_OGLRES_LABEL, "texture: failed delete, not found key.");
			return false;
		}
	}

	VertexBufferAttr ImFXOGLresVertexBuffer::ResourceFind(ResUnique key) {
		std::lock_guard<std::mutex> Lock(ResourceVertexBufferMutex);
		return (ResourceVertexBufferMap.find(key) != ResourceVertexBufferMap.end()) ? ResourceVertexBufferMap[key] : VertexBufferAttr();
	}

	bool ImFXOGLresVertexBuffer::ResourceStorage(ResUnique key, ImFxGLmanagerModel* res) {
		std::lock_guard<std::mutex> Lock(ResourceVertexBufferMutex);

		auto it = ResourceVertexBufferMap.find(key);
		if (it != ResourceVertexBufferMap.end()) {
			ImFXLog(LOG_WARNING, IMFX_OGLRES_LABEL, "verbuffer: failed storage duplicate_key: %s", key.c_str());
			return false;
		}
		else {
			ResourceFlag ResFlag = {};
			auto ResStorage = res->_MS_GETRES(ResFlag);
			// resource = normal ?
			if (ResFlag == DEFRES_FLAG_NORMAL) {
				ResourceVertexBufferMap[key] = ResStorage;
				ImFXLog(LOG_INFO, IMFX_OGLRES_LABEL, "verbuffer: storage key: %s", key.c_str());
				return true;
			}
			else {
				ImFXLog(LOG_ERROR, IMFX_OGLRES_LABEL, "verbuffer: failed storage res_err key: %s, code: %i", key.c_str(), ResFlag);
				return false;
			}
		}
	}

	bool ImFXOGLresVertexBuffer::ResourceDelete(ResUnique key) {
		std::lock_guard<std::mutex> Lock(ResourceVertexBufferMutex);

		auto it = ResourceVertexBufferMap.find(key);
		if (it != ResourceVertexBufferMap.end()) {

			// delete vbo handle => clear map_item. (VAO只为映射所以不由此管理)
			glDeleteBuffers(1, &it->second.DataBuffer);
			ResourceVertexBufferMap.erase(it);

			ImFXLog(LOG_INFO, IMFX_OGLRES_LABEL, "verbuffer: delete key: %s", key.c_str());
			return true;
		}
		else {
			ImFXLog(LOG_WARNING, IMFX_OGLRES_LABEL, "verbuffer: failed delete, not found key.");
			return false;
		}
	}

	VertexBufferAttr* ImFXOGLresVertexBuffer::ExtResourceMapping(ResUnique key) {
		std::lock_guard<std::mutex> Lock(ResourceVertexBufferMutex);
		return (ResourceVertexBufferMap.find(key) != ResourceVertexBufferMap.end()) ? &ResourceVertexBufferMap[key] : nullptr;
	}

	ImfxVertexAttribute ImFXOGLresVertexAttr::ResourceFind(ResUnique key) {
		std::lock_guard<std::mutex> Lock(ResourceVertexAttrMutex);
		return (ResourceVertexAttrMap.find(key) != ResourceVertexAttrMap.end()) ? ResourceVertexAttrMap[key] : NULL;
	}

	bool ImFXOGLresVertexAttr::ResourceStorage(ResUnique key, ImfxVertexAttribute res) {
		std::lock_guard<std::mutex> Lock(ResourceVertexAttrMutex);

		auto it = ResourceVertexAttrMap.find(key);
		if (it != ResourceVertexAttrMap.end()) {
			ImFXLog(LOG_WARNING, IMFX_OGLRES_LABEL, "veratt: failed storage duplicate_key: %s", key.c_str());
			return false;
		}
		else {
			ResourceVertexAttrMap[key] = res;
			ImFXLog(LOG_INFO, IMFX_OGLRES_LABEL, "veratt: storage key: %s", key.c_str());
			return true;
		}
	}

	bool ImFXOGLresVertexAttr::ResourceDelete(ResUnique key) {
		std::lock_guard<std::mutex> Lock(ResourceVertexAttrMutex);

		auto it = ResourceVertexAttrMap.find(key);
		if (it != ResourceVertexAttrMap.end()) {

			// delete vao handle => clear map_item.
			glDeleteVertexArrays(1, &it->second);
			ResourceVertexAttrMap.erase(it);

			ImFXLog(LOG_INFO, IMFX_OGLRES_LABEL, "veratt: delete key: %s", key.c_str());
			return true;
		}
		else {
			ImFXLog(LOG_WARNING, IMFX_OGLRES_LABEL, "veratt: failed delete, not found key.");
			return false;
		}
	}

	ImfxFrameBuffer ImFXOGLresFrameBuffer::ResourceFind(ResUnique key) {
		std::lock_guard<std::mutex> Lock(ResourceFrameBufferMutex);
		return (ResourceFrameBufferMap.find(key) != ResourceFrameBufferMap.end()) ? ResourceFrameBufferMap[key] : NULL;
	}

	bool ImFXOGLresFrameBuffer::ResourceStorage(ResUnique key, ImFxGLmanagerFrameBuffer* res) {
		std::lock_guard<std::mutex> Lock(ResourceFrameBufferMutex);

		auto it = ResourceFrameBufferMap.find(key);
		if (it != ResourceFrameBufferMap.end()) {
			ImFXLog(LOG_WARNING, IMFX_OGLRES_LABEL, "framebuf: failed storage duplicate_key: %s", key.c_str());
			return false;
		}
		else {
			ResourceFlag ResFlag = {};
			auto ResStorage = res->_MS_GETRES(ResFlag);
			// resource = normal ?
			if (ResFlag == DEFRES_FLAG_NORMAL) {
				ResourceFrameBufferMap[key] = ResStorage;
				ImFXLog(LOG_INFO, IMFX_OGLRES_LABEL, "framebuf: storage key: %s", key.c_str());
				return true;
			}
			else {
				ImFXLog(LOG_ERROR, IMFX_OGLRES_LABEL, "framebuf: failed storage res_err key: %s, code: %i", key.c_str(), ResFlag);
				return false;
			}
		}
	}

	bool ImFXOGLresFrameBuffer::ResourceDelete(ResUnique key) {
		std::lock_guard<std::mutex> Lock(ResourceFrameBufferMutex);

		auto it = ResourceFrameBufferMap.find(key);
		if (it != ResourceFrameBufferMap.end()) {

			// delete fbo handle => clear map_item.
			glDeleteFramebuffers(1, &it->second);
			ResourceFrameBufferMap.erase(it);

			ImFXLog(LOG_INFO, IMFX_OGLRES_LABEL, "framebuf: delete key: %s", key.c_str());
			return true;
		}
		else {
			ImFXLog(LOG_WARNING, IMFX_OGLRES_LABEL, "framebuf: failed delete, not found key.");
			return false;
		}
	}
}