#include "pkgio.h"
#include <sys/stat.h>
#include <stdio.h>
#include "ccattribs.h"
#include "ccattribs.h"
#include <algorithm>

using namespace LibTrident;
using namespace LibTrident::PkgIO;
using namespace LibTrident::PkgIO::FileOperations;

bool _ICPU_IsLittleEndian() {

    int i=1;

    return (int)*((unsigned char *)&i)==1;

}

std::string FileOperations::GetFileDir(const std::string& file) {
    size_t pos = file.find_last_of('/');
    if (pos == std::string::npos) {
        return "";
    }
    return file.substr(0, pos);
}

std::streamsize FileOperations::GetFstreamSize(std::shared_ptr<std::fstream> fs) {
    if (!fs->is_open()) {
        return -1;
    }
    std::streampos orignalPosition = fs->tellg();
    std::streampos fileSize;
    if (orignalPosition == -1) {
        return -1;
    }
    fs->seekg(0, std::ios::end);
    if (!fs) {
         goto fail;
         
    }
    fileSize = fs->tellg();
    if (fileSize == -1) {
        goto fail;
    }
    //reset to the original state
    fs->seekg(orignalPosition, std::ios::beg);
    return fileSize;

fail:
    fs->seekg(orignalPosition, std::ios::beg);
    return -1;

  
}
bool FileOperations::GetFileStats(const char* file, struct stat64& statOut) {
    //yes i know on x86 stat is always evaluated to stat64, better be safe then sorry
    struct stat64 fileStat = { };
  
    bool ret = false;
    if (file && stat64(file, &fileStat) == 0) {
        
        ret = true;
    };
    //most likely not the best approach, but i dont want to use pointers
    statOut = fileStat;
    return ret;
        
}

LibTrident::LTSTATUS::LTSTATUS FileOperations::FileOrDirExists(const std::string& path, bool file) {
    
    //handle scenarios like /tmp/folder/
    std::string fullPath = path;
    if (!file) {
        if(fullPath.back() == '/') {
            size_t pos = path.find_last_of("/");
            std::string fullPath  = path.substr(0, pos);
            fullPath = path.substr(0, pos - 1);
    }
}
     struct stat64 dStat;
    if (!GetFileStats(path.c_str(), dStat)) {
      //  e.SetError(LTSTATUS::INVFILE);
        return LTSTATUS::INVFILE;
    }
    if (!file) {
        if (!S_ISDIR(dStat.st_mode)) {
             dbgprintf("stat(D_%s) fail\n", fullPath.c_str());
            return LTSTATUS::NOTDIR;
        }
    }
    else {
        if (!S_ISREG(dStat.st_mode)) {
            dbgprintf("stat(R_%s) fail\n", fullPath.c_str());
            return LTSTATUS::NOTFILE;
        }
    }
   

    return LTSTATUS::SUCCESS;
}
void ReverseByteOrder( char* start, size_t size )
{
    char* byteEnd = start + size;
    std::reverse(start, byteEnd);
}
bool FileOperations::WriteLeStream(std::shared_ptr<std::fstream> stream,const char* data, std::streamsize size){
    if (!stream->is_open() || !data || size < 1) {
        dbgprintf("Error: WriteLeStream() Failed\n");
        return false;
    }
    if (_ICPU_IsLittleEndian()) {
        stream->write(data, size);
    }
    else {
        char* buffer = new char[size];
        if (!buffer) {
            return false;
        }
        std::copy(data, data + size, buffer);
        ReverseByteOrder(buffer, size);
        stream->write(buffer, size);
        delete[] buffer;
    }
    if (stream) {
        return true;
    }
    dbgprintf("Error: WriteLeStream() Failed\n");
    return !stream;
}
            
 bool FileOperations::WriteLeStream(FstreamInfo::TRDFstreamObject& info, const char* data, std::streamsize size, bool increment) {
    if (FstreamInfo::TrdFstreamInfo::ValidateRemoteFileStreamInfo(info) != LTSTATUS::SUCCESS)  {
        dbgprintf("Error: WriteLeStream(validate) Failed\n");
        return false;
    }
   
    bool st = WriteLeStream(info.hFile, data, size);
    if (st && increment) {
        info.fSize += size;
    }
    return st;
}

bool FileOperations::ReadLeStream(std::shared_ptr<std::fstream> stream, char* s, std::streamsize size) {
    if (!stream->is_open() || !s || size < 1) {
        dbgprintf("Error: ReadLeStream() Failed\n");
        return false;
    }
    char* data = new char[size];
    if (!data) {
        return false;
    }
    
    if (_ICPU_IsLittleEndian()) {
        stream->read(data, size);
    }
    else {
        stream->read(data, size);
        ReverseByteOrder(data, size);
        
    }


      if (!stream) {
        delete[] data;
        return false;
    }
    std::copy(data, data + size, s);

    delete[] data;
    return true;
}
 bool FileOperations::ReadLeStream(FstreamInfo::TRDFstreamObject& info, char* s, std::streamsize size) {
    if (FstreamInfo::TrdFstreamInfo::ValidateRemoteFileStreamInfo(info) != LTSTATUS::SUCCESS) {
        return false;
    }
    return ReadLeStream(info.hFile, s, size);
}

//todo use ReadLeStream()
