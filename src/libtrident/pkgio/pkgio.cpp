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
Err::Code BinarySerializer::WriteDataToTStream(std::weak_ptr<LibTrident::Tstream::TStreamInfo> wFstr, const std::vector<u8>& data, i64 seekPos, std::ios_base::seekdir seekDir) {
    //todo make this boilerplate in all classes a function
    if (data.empty()) {
        return Err::Code::BADARG;
    }
    auto haveCtx = Tstream::TStreamInfo::GetFstreamContent(wFstr);
    if (!haveCtx.has_value()) {
        return Err::Code::IREF_EXPIRED;
    }
    auto tStream = haveCtx.value();
    if (!tStream->CheckFileStreamInfo()) {
        return tStream->e.GetError();
    }
    i64 ogSeek = tStream->GetSeekPosW();
    if (ogSeek == -1 || seekPos < 0) {
        return Err::Code::FSEEK;
    }
    if (!tStream->SetSeekPosW(seekPos, seekDir)) {
        return Err::Code::FSEEK;
    }
    if (!ExpectAlignedDataOrDie(data.size())) {
        return Err::Code::BADDATA;
    }

    bool st = tStream->WriteTStream(reinterpret_cast<const char*>(data.data()), data.size(),  true);
    bool seek = tStream->SetSeekPosW(ogSeek);
    if (!st) {
        return Err::Code::IO_WRITE;
    }
    if (!seek) {
        return Err::Code::FSEEK;
    }

    return Err::Code::SUCCESS;
}

std::optional<std::vector<u8>> BinarySerializer::GetFormattedData(bool autoAlign) {
    if (bufferData.empty()) {
        return std::nullopt;
    }
   
    size_t alignSize = 0;

    if (autoAlign) {
        const auto& vSize = bufferData.size();
        if (!IsDataSizeAligned(vSize)) {
            alignSize = GetByteAlignment(vSize) - vSize;
            dbgprintf("--Unaligned data serialized\nog:%luB new: %luB\n", vSize, alignSize+vSize);
        }
    }

    bufferData.insert(bufferData.end(), alignSize, 0);
    if (!ExpectAlignedDataOrDie(bufferData.size())) {
            return std::nullopt;
    }
    dbgprintf("--Serializing data size %luB\n\n", bufferData.size());
   // bool st = tStream->WriteTStream(reinterpret_cast<const char*>(bufferData.data()), bufferData.size(),  true);
    
    return bufferData;

    
    

}


//todo use ReadLeStream()
