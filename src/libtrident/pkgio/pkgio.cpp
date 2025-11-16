#include "pkgio.h"
#include <sys/stat.h>
#include <stdio.h>
#include "ccattribs.h"
#include "ccattribs.h"
#include <cerrno>

using namespace LibTrident;
using namespace LibTrident::PkgIO;
using namespace LibTrident::PkgIO::FileOperations;
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#error "The implementation on Big Endian is currently completely broken. DO NOT USE THIS PROGRAM ON BIG ENDIAN SYSTEM"
#endif


std::streamsize FileOperations::GetFstreamSize(std::shared_ptr<std::fstream> fs) {
    if (!fs->is_open()) {
        throw std::ios_base::failure("Failed to open fd");
    }
    std::streampos orignalPosition = fs->tellg();
    std::streampos fileSize;
    if (orignalPosition == -1) {
        throw std::ios_base::failure("tellg() failure");
    }
    fs->seekg(0, std::ios::end);
    if (!fs) {
         throw std::ios_base::failure("seekg() failure");
         
    }
    fileSize = fs->tellg();
    if (fileSize == -1) {
        throw std::ios_base::failure("tellg() failure");
    }
    //reset to the original state
    fs->seekg(orignalPosition, std::ios::beg);
    if (!fs) {
        throw std::ios_base::failure("seekg() failure");
    }
    return fileSize;
  
}
std::shared_ptr<struct stat64> FileOperations::GetFileStats(const std::filesystem::path& file) {
    //yes i know on x86 stat is always evaluated to stat64, better be safe then sorry
    std::shared_ptr<struct stat64> fileStat = std::make_shared<struct stat64>();
    if (stat64(file.c_str(), fileStat.get()) == 0) {
        
        return fileStat;
        
    };
    throw std::system_error(errno, std::generic_category(), "stat64() failed on " + file.string());
}




void FileOperations::WriteLeData(std::shared_ptr<std::fstream> stream,const char* data, std::streamsize size){
    if (!stream->is_open() || !data || size < 1) {
        throw std::ios_base::failure("WriteLeData() Failed");
    }
    //THIS WONT WORK ON BIG ENDIAN AT ALL, IT ONLY WORKS FOR TRIVIAL TYPES NOT FOR STRUCTS
    //WE NEED TO SERIALIZE THE STRUCT BEFORE WRITING IT, OTHERWISE IT PRODUCES GARBAGE

     ///endian operations will be moved entirely to BinarySerializer class
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
    if (!stream) {
        throw std::ios_base::failure("WriteLeData() Failed");
    }
}
            


void FileOperations::ReadLeData(std::shared_ptr<std::fstream> stream, char* s, std::streamsize size) {
    if (!stream->is_open() || !s || size < 1) {
        throw std::ios_base::failure("ReadLeStream() Failed\n");
    }
    char* data = new char[size];
    if (!data) {
        throw std::bad_alloc();
    }
    ///endian operations will be moved entirely to BinarySerializer class
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
        throw std::ios_base::failure("Error: ReadLeStream() Failed\n");
    }
    std::copy(data, data + size, s);
    delete[] data;
}


void BinarySerializer::AddRaw(const void*  data, size_t size) {
    if (!data || size == 0) {
        throw std::invalid_argument("AddRaw() failed. Data or size is 0");
    }
    const uint8_t* bytes = reinterpret_cast<const uint8_t*>(data);
    bufferData.insert(bufferData.end(), bytes, bytes + size);

}
void BinarySerializer::WriteDataToTStream(std::weak_ptr<LibTrident::Tstream::TStreamInfo> wFstr, const std::vector<u8>& data, i64 seekPos, std::ios_base::seekdir seekDir) {
    //todo make this boilerplate in all classes a function
    if (data.empty()) {
        throw std::invalid_argument("Empty buffer was passed");
    }
    auto haveCtx = Tstream::TStreamInfo::GetFstreamContent(wFstr);
    if (!haveCtx.has_value()) {
        throw std::runtime_error("Weak pointer reference expired");
    }
    auto tStream = haveCtx.value();
    if (!tStream->CheckFileStreamInfo()) {
        throw std::runtime_error("CheckFileStreamInfo() failed");
    }
    i64 ogSeek = tStream->GetSeekPosW();

    tStream->SetSeekPosW(seekPos, seekDir);
    if (!ExpectAlignedDataOrDie(data.size())) {
        throw std::invalid_argument("Unaligned data");
    }

    tStream->WriteTStream(reinterpret_cast<const char*>(data.data()), data.size(),  true);
    tStream->SetSeekPosW(ogSeek);

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
