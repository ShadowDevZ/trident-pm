#include "pkgio.h"
#include <sys/stat.h>
#include <stdio.h>
#include "ccattribs.h"
#include "ccattribs.h"
#include <algorithm>

using namespace LibTrident;
using namespace LibTrident::PkgIO;
using namespace LibTrident::PkgIO::FileOperations;
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#error "The implementation on Big Endian is currently completely broken. DO NOT USE THIS PROGRAM ON BIG ENDIAN SYSTEM"
#endif
bool _ICPU_IsLittleEndian() {

    int i=1;

    return (int)*((unsigned char *)&i)==1;

}
/*
std::string FileOperations::GetFileDir(const std::string& file) {
    size_t pos = file.find_last_of('/');
    if (pos == std::string::npos) {
        return "";
    }
    return file.substr(0, pos);
}
*/

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
bool FileOperations::GetFileStats(const std::filesystem::path& file, struct stat64& statOut) {
    //yes i know on x86 stat is always evaluated to stat64, better be safe then sorry
    struct stat64 fileStat = { };
  
    bool ret = false;
    if (stat64(file.c_str(), &fileStat) == 0) {
        
        ret = true;
    };
    //most likely not the best approach, but i dont want to use pointers
    statOut = fileStat;
    return ret;
        
}

void ReverseByteOrder( char* start, size_t size )
{
    char* byteEnd = start + size;
    std::reverse(start, byteEnd);
}
bool FileOperations::WriteLeData(std::shared_ptr<std::fstream> stream,const char* data, std::streamsize size){
    if (!stream->is_open() || !data || size < 1) {
        dbgprintf("Error: WriteLeStream() Failed\n");
        return false;
    }
    //THIS WONT WORK ON BIG ENDIAN AT ALL, IT ONLY WORKS FOR TRIVIAL TYPES NOT FOR STRUCTS
    //WE NEED TO SERIALIZE THE STRUCT BEFORE WRITING IT, OTHERWISE IT PRODUCES GARBAGE
    if (_ICPU_IsLittleEndian()) {
        stream->write(data, size);
    }
    /* BROKEN
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
    */
    if (stream) {
        return true;
    }
    dbgprintf("Error: WriteLeStream() Failed\n");
    return !stream;
}
            


bool FileOperations::ReadLeData(std::shared_ptr<std::fstream> stream, char* s, std::streamsize size) {
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
    /*BROKEN
    else {
        stream->read(data, size);
        ReverseByteOrder(data, size);
        
    }
    */

      if (!stream) {
        delete[] data;
        return false;
    }
    std::copy(data, data + size, s);

    delete[] data;
    return true;
}

//todo use ReadLeStream()
