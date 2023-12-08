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

        ELog(LogInfo, MODULE_FILELOADER, "loader(bin) open file: %s read_size: %u", filename.c_str(), read_filesize);
        file.close();
    }
    else
        ELog(LogError, MODULE_FILELOADER, "failed loader(bin) open file: %s", filename.c_str());
}

bool FileLoaderBinary::WriterFileBinary(const string& filename, const vector<uint8_t>& databin, ios_base::openmode mode) {
    ofstream file(filename, ios::binary | mode);
    if (file.is_open()) {
        // write binary data. 
        file.write(reinterpret_cast<const char*>(databin.data()), databin.size());

        ELog(LogInfo, MODULE_FILELOADER, "writer(bin) open file: %s write_size: %u", filename.c_str(), databin.size());
        file.close();
        return true;
    }
    else {
        ELog(LogError, MODULE_FILELOADER, "failed writer(bin) open file: %s", filename.c_str());
        return false;
    }
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

        // read string data.
        string fileContent((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
        read_filedata = fileContent;

        ELog(LogInfo, MODULE_FILELOADER, "loader(str) open file: %s read_size: %u", filename.c_str(), read_filesize);
    }
    else
        ELog(LogError, MODULE_FILELOADER, "failed loader(str) open file: %s", filename.c_str());
}

bool FileLoaderString::WriterFileString(const string& filename, const string& databin, ios_base::openmode mode) {
    fstream file(filename, ios::binary | mode);
    if (file.is_open()) {
        // write string data. 
        file.write(databin.data(), databin.size());

        ELog(LogInfo, MODULE_FILELOADER, "writer(str) open file: %s write_size: %u", filename.c_str(), databin.size());
        file.close();
        return true;
    }
    else {
        ELog(LogError, MODULE_FILELOADER, "failed writer(str) open file: %s", filename.c_str());
        return false;
    }
}

string FileLoaderString::GetDataString() {
    return read_filedata;
}

size_t FileLoaderString::GetFileSize() {
    return read_filesize;
}