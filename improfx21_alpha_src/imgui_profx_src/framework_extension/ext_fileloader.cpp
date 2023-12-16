// ext_fileloader.
#include "ext_fileloader.h"

using namespace std;
using namespace ExtLog;

#define MODULE_FILELOADER "[fileloader]: "

FileLoaderBinary::FileLoaderBinary(const string& filename) {
    ifstream ReadFile(filename, ios::binary);
    if (ReadFile.is_open()) {
        // get file size.
        ReadFile.seekg(0, ios::end);
        size_t ReadFileSize = (size_t)ReadFile.tellg();
        ReadFile.seekg(0, ios::beg);

        // read binary data.
        ReadFileData.resize(ReadFileSize);
        ReadFile.read(reinterpret_cast<char*>(ReadFileData.data()), ReadFileSize);

        ELog(LogInfo, MODULE_FILELOADER, "loader(bin) open file: %s read_size: %u", filename.c_str(), ReadFileSize);
        ReadFile.close();
    }
    else
        ELog(LogError, MODULE_FILELOADER, "failed loader(bin) open file: %s", filename.c_str());
}

bool FileLoaderBinary::WriterFileBinary(const string& filename, const vector<uint8_t>& databin, ios_base::openmode mode) {
    ofstream WriteFile(filename, ios::binary | mode);
    if (WriteFile.is_open()) {
        // write binary data. 
        WriteFile.write(reinterpret_cast<const char*>(databin.data()), databin.size());

        ELog(LogInfo, MODULE_FILELOADER, "writer(bin) open file: %s write_size: %u", filename.c_str(), databin.size());
        WriteFile.close();
        return true;
    }
    else {
        ELog(LogError, MODULE_FILELOADER, "failed writer(bin) open file: %s", filename.c_str());
        return false;
    }
}

vector<uint8_t> FileLoaderBinary::GetDataBinary() {
    return ReadFileData;
}

size_t FileLoaderBinary::GetFileSize() {
    return ReadFileSize;
}

FileLoaderString::FileLoaderString(const string& filename) {
    ifstream ReadFile(filename, ios::binary);
    if (ReadFile.is_open()) {
        // get file size.
        ReadFile.seekg(0, ios::end);
        size_t ReadFileSize = (size_t)ReadFile.tellg();
        ReadFile.seekg(0, ios::beg);

        // read string data.
        string FileContent((istreambuf_iterator<char>(ReadFile)), istreambuf_iterator<char>());
        ReadFileData = FileContent;

        ELog(LogInfo, MODULE_FILELOADER, "loader(str) open file: %s read_size: %u", filename.c_str(), ReadFileSize);
    }
    else
        ELog(LogError, MODULE_FILELOADER, "failed loader(str) open file: %s", filename.c_str());
}

bool FileLoaderString::WriterFileString(const string& filename, const string& databin, ios_base::openmode mode) {
    fstream WriteFile(filename, ios::binary | mode);
    if (WriteFile.is_open()) {
        // write string data. 
        WriteFile.write(databin.data(), databin.size());

        ELog(LogInfo, MODULE_FILELOADER, "writer(str) open file: %s write_size: %u", filename.c_str(), databin.size());
        WriteFile.close();
        return true;
    }
    else {
        ELog(LogError, MODULE_FILELOADER, "failed writer(str) open file: %s", filename.c_str());
        return false;
    }
}

string FileLoaderString::GetDataString() {
    return ReadFileData;
}

size_t FileLoaderString::GetFileSize() {
    return ReadFileSize;
}