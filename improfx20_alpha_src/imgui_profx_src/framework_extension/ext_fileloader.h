// ext_fileloader. RCSZ 2023_12_3.

#ifndef _EXT_FILELOADER_H
#define _EXT_FILELOADER_H
#include "framework_extension_tools.hpp"

// 二进制文件加载器.
class FileLoaderBinary {
protected:
    std::vector<uint8_t> read_filedata = {};
    size_t               read_filesize = {};
public:
    FileLoaderBinary(const std::string& filename);
    FileLoaderBinary() {};

    bool WriterFileBinary(
        const std::string&          filename, 
        const std::vector<uint8_t>& databin, 
        std::ios_base::openmode     mode = std::ios_base::out
    );

    std::vector<uint8_t> GetDataBinary();
    size_t               GetFileSize();
};

// 文本文件加载器.
class FileLoaderString {
protected:
    std::string read_filedata = {};
    size_t      read_filesize = {};
public:
    FileLoaderString(const std::string& filename);
    FileLoaderString() {};

    bool WriterFileString(const std::string& filename, const std::string& databin, std::ios_base::openmode mode = std::ios_base::out);

    std::string GetDataString();
    size_t      GetFileSize();
};

#endif