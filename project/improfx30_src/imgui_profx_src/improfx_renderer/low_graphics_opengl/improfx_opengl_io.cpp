// improfx_opengl_io.
#include "improfx_opengl.hpp"

namespace IMFX_OGL_IOP {
	// libray: stb_image,stb_image_write
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define __STDC_LIB_EXT1__ // "sprintf" => "sprintf_s" _CRT_SECURE_NO_WARNINGS
#include "stb_image_write.h"

	bool ImFXOGLioImage::WriteImageFile(ImageRawData rawdata, std::string file, SaveImageMode mode, float quality) {
		std::filesystem::path SaveFilePath(file);
		std::filesystem::path DirectoryPath = SaveFilePath.parent_path();

		auto WriteImageFlagFunc = [&](bool flag, const char* label, const char* file) {
			// process stbi_write return_flag.
			if (flag) ImFXLog(LOG_INFO, IMFX_OGLIOP_LABEL, "save(%s) image_file: %s", label, file);
			else      ImFXLog(LOG_WARNING, IMFX_OGLIOP_LABEL, "failed save(%s) image_file: %s", label, file);
		};

		if (std::filesystem::is_directory(DirectoryPath)) {
			switch (mode) {
			case(ImageJPG): {
				bool WriteFlag = (bool)stbi_write_jpg(
					file.c_str(),
					rawdata.Width, rawdata.Height, rawdata.Channels, rawdata.ImagePixels.data(),
					int(quality * 100.0f)
				);
				WriteImageFlagFunc(WriteFlag, ".jpg", file.c_str());
				return WriteFlag;
			}
			case(ImagePNG): {
				bool WriteFlag = (bool)stbi_write_png(
					file.c_str(),
					rawdata.Width, rawdata.Height, rawdata.Channels, rawdata.ImagePixels.data(),
					int(quality * 100.0f)
				);
				WriteImageFlagFunc(WriteFlag, ".png", file.c_str());
				return WriteFlag;
			}
			// write: invalid mode.
			default: { ImFXLog(LOG_WARNING, IMFX_OGLIOP_LABEL, "invalid mode, save image."); return false; }
			}
		}
		else {
			ImFXLog(LOG_WARNING, IMFX_OGLIOP_LABEL, "invalid directory, save image_file: %s", DirectoryPath.string().c_str());
			return false;
		}
	}

	ImageRawData ImFXOGLioImage::ReadImageFile(std::string file) {
		ImageRawData ReturnRawImageData = {};

		if (std::filesystem::exists(file)) {
			// stbi_image loader.
			int width = 0, height = 0, channels = 0;
			uint8_t* RawDataPtr = stbi_load(file.c_str(), &width, &height, &channels, NULL);;

			// stbi_load return nullptr: failed load.
			if (RawDataPtr != nullptr) {

				ImFXLog(LOG_INFO, IMFX_OGLIOP_LABEL, "read image: image file: %s", file.c_str());
				ImFXLog(LOG_INFO, IMFX_OGLIOP_LABEL, "read image: image size: %d x %d", width, height);
				ImFXLog(LOG_INFO, IMFX_OGLIOP_LABEL, "read image: image color_channel: %d", channels);

				ReturnRawImageData.Channels = channels;
				ReturnRawImageData.Width    = width;
				ReturnRawImageData.Height   = height;
				// copy raw_image memory.
				ReturnRawImageData.ImagePixels.insert(
					ReturnRawImageData.ImagePixels.begin(),
					RawDataPtr,
					RawDataPtr + size_t(width * height * channels)
				);
				// free loader memory.
				stbi_image_free(RawDataPtr);
			}
			else
				ImFXLog(LOG_WARNING, IMFX_OGLIOP_LABEL, "failed load, raw_dataptr = nullptr.", file.c_str());
		}
		else
			ImFXLog(LOG_WARNING, IMFX_OGLIOP_LABEL, "failed open, read image_file: %s", file.c_str());
		return ReturnRawImageData;
	}
}