#include "pkgio.h"
#include <sys/stat.h>
#include <stdio.h>
#include "ccattribs.h"
#include "ccattribs.h"


using namespace LibTrident;
using namespace LibTrident::PkgIO;
using namespace LibTrident::PkgIO::FileOperations;
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#error "The implementation on Big Endian is currently completely broken. DO NOT USE THIS PROGRAM ON BIG ENDIAN SYSTEM"
#endif


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




bool FileOperations::WriteLeData(std::shared_ptr<std::fstream> stream,const char* data, std::streamsize size){
    if (!stream->is_open() || !data || size < 1) {
        dbgprintf("Error: WriteLeStream() Failed\n");
        return false;
    }
    //THIS WONT WORK ON BIG ENDIAN AT ALL, IT ONLY WORKS FOR TRIVIAL TYPES NOT FOR STRUCTS
    //WE NEED TO SERIALIZE THE STRUCT BEFORE WRITING IT, OTHERWISE IT PRODUCES GARBAGE
    if (BinarySerializer::IsLittleEndianArch()) {
        stream->write(data, size);
    }
    else {
        //failsafe
        assert(0 && "Unsupported operation on BE");
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
    
    if (BinarySerializer::IsLittleEndianArch()) {
        stream->read(data, size);
    }
    else {
        //failsafe
        assert(0 && "Unsupported operation on BE");
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


bool BinarySerializer::AddRaw(const void*  data, size_t size) {
    if (!data || size == 0) {
        return false;
    }
    const uint8_t* bytes = reinterpret_cast<const uint8_t*>(data);
    bufferData.insert(bufferData.end(), bytes, bytes + size);
    return true;

}

bool BinarySerializer::WriteData(i64 seekPos, std::ios_base::seekdir seekDir) {
    //todo make this boilerplate in all classes a function
    auto haveCtx = Tstream::TStreamInfo::GetFstreamContent(wFstr);
    if (!haveCtx.has_value()) {
        return false;
    }
    auto tStream = haveCtx.value();
    if (!tStream->CheckFileStreamInfo()) {
        return false;
    }
    i64 ogSeek = tStream->GetSeekPosW();
    if (ogSeek == -1 || seekPos < 0) {
        return false;
    }
    if (!tStream->SetSeekPosW(seekPos, seekDir)) {
        return false;
    }
    size_t alignSize = 0;
    //todo use assertion in other function, quick fix
    if (!IsDataSizeAligned(bufferData.size(), false)) {
        alignSize = GetByteAlignment(bufferData.size()) - bufferData.size();
    }
    bufferData.insert(bufferData.end(), alignSize, 0);
    dbgprintf("\n\n\n%lu\n", bufferData.size());
    bool st = tStream->WriteTStream(reinterpret_cast<const char*>(bufferData.data()), bufferData.size(),  true);
    tStream->SetSeekPosW(ogSeek);
    return st;

    
    

}
//todo use ReadLeStream()
