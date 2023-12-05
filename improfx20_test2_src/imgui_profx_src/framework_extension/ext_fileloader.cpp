// ext_fileloader.
#include "ext_fileloader.h"

using namespace std;
using namespace ExtLog;

#define MODULE_FILELOADER "[fileloader]: "

FileLoaderBinary::FileLoaderBinary(const string& filename) {
    ifstream file(filename, ios::binary);
    if (file.is_open()) {
        // get file size.
        file.seekg(0, ios::end);
        size_t read_filesize = (size_t)file.tellg();
        file.seekg(0, ios::beg);

        // read binary data.
        read_filedata.resize(read_filesize);
        file.read(reinterpret_cast<char*>(read_filedata.data()), read_filesize);

        ELog(LogInfo, MODULE_FILELOADER, "loader(bin) open file: %s read_size: %u", filename, read_filesize);
    }
    else
        ELog(LogError, MODULE_FILELOADER, "failed loader(bin) open file: %s", filename);
}

vector<uint8_t> FileLoaderBinary::GetDataBinary() {
    return read_filedata;
}

size_t FileLoaderBinary::GetFileSize() {
    return read_filesize;
}

FileLoaderString::FileLoaderString(const string& filename) {
    ifstream file(filename, ios::binary);
    if (file.is_open()) {
        // get file size.
        file.seekg(0, ios::end);
        size_t read_filesize = (size_t)file.tellg();
        file.seekg(0, ios::beg);

        // read binary data.
        string fileContent((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
        read_filedata = fileContent;

        ELog(LogInfo, MODULE_FILELOADER, "loader(str) open file: %s read_size: %u", filename, read_filesize);
    }
    else
        ELog(LogError, MODULE_FILELOADER, "failed loader(str) open file: %s", filename);
}

string FileLoaderString::GetDataString() {
    return read_filedata;
}

size_t FileLoaderString::GetFileSize() {
    return read_filesize;
}