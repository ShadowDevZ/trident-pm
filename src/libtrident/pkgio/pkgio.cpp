#include "pkgio.h"
#include <sys/stat.h>
#include <stdio.h>
#include "ccattribs.h"
#include "ccattribs.h"
#include <cerrno>
#include <format>
#include "filemgmnt.h"
using namespace LibTrident;
using namespace LibTrident::PkgIO;
using namespace LibTrident::PkgIO::FileOperations;
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#error "The implementation on Big Endian is currently completely broken. DO NOT USE THIS PROGRAM ON BIG ENDIAN SYSTEM"
#endif




void FileOperations::writeLeData(std::shared_ptr<std::fstream> stream,const char* data, std::streamsize size){
    if (!stream->is_open() || !data || size < 1) {
        throw std::ios_base::failure("WriteLeData() Failed");
    }
    //THIS WONT WORK ON BIG ENDIAN AT ALL, IT ONLY WORKS FOR TRIVIAL TYPES NOT FOR STRUCTS
    //WE NEED TO SERIALIZE THE STRUCT BEFORE WRITING IT, OTHERWISE IT PRODUCES GARBAGE

     ///endian operations will be moved entirely to BinarySerializer class
    if (BinarySerializer::isLittleEndian()) {
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
            


void FileOperations::readLeData(std::shared_ptr<std::fstream> stream, char* s, std::streamsize size) {
    if (!stream->is_open() || !s || size < 1) {
        throw std::ios_base::failure("ReadLeStream() Failed\n");
    }
    char* data = new char[size];
    if (!data) {
        throw std::bad_alloc();
    }
    ///endian operations will be moved entirely to BinarySerializer class
    if (BinarySerializer::isLittleEndian()) {
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


void BinarySerializer::addRaw(const void*  data, size_t size) {
    if (!data || size == 0) {
        throw std::invalid_argument("AddRaw() failed. Data or size is 0");
    }
    const uint8_t* bytes = reinterpret_cast<const uint8_t*>(data);
    bufferData.insert(bufferData.end(), bytes, bytes + size);

}
#include <cstring>
size_t BinarySerializer::readRaw(void*  dataOut, size_t size, size_t offset) {
    if (dataOut == nullptr) {
        throw std::invalid_argument("nullptr was passed");
    }
    if (offset + size > bufferData.size()) {
        std::string pi = std::format("buffsz: {}, exp_atl: {}", bufferData.size(), offset+size);
        throw std::out_of_range("Buffer was not big enough " + pi);
    }
    
    if (size == 0) {
        throw std::runtime_error("Read 0 bytes");
    }
    uint8_t* bytes = reinterpret_cast<uint8_t*>(dataOut);
    std::memcpy(bytes, bufferData.data() + offset, size);


    return size;

}

std::vector<u8> BinarySerializer::readDataFromTStream(LibTrident::Tstream::TStreamInfo& tStream, i64 seekPos, u64 size, bool checkAlignment) {
   
    if (!checkAlignment || !isDataSizeAligned(size)) {
        throw std::invalid_argument("Data size not aligned");
    }
    if (!tStream.checkFileStreamInfo()) {
        throw std::runtime_error("CheckFileStreamInfo() failed");
    }
    i64 ogSeek = tStream.getSeekPos();
    tStream.setSeekPos(seekPos);
    std::vector<u8> data(size);
   // data.reserve(size);

    tStream.readTStream(reinterpret_cast<char*>(data.data()),  size);
    tStream.setSeekPos(ogSeek);
    return data;

}
void BinarySerializer::writeDataToTStream(LibTrident::Tstream::TStreamInfo& tStream, const std::vector<u8>& data, i64 seekPos, std::ios_base::seekdir seekDir) {
    //todo make this boilerplate in all classes a function
    if (data.empty()) {
        throw std::invalid_argument("Empty buffer was passed");
    }
    if (!tStream.checkFileStreamInfo()) {
        throw std::runtime_error("CheckFileStreamInfo() failed");
    }
    i64 ogSeek = tStream.getSeekPos();

    tStream.setSeekPos(seekPos, seekDir);
    if (!expectAlignedDataOrDie(data.size())) {
        throw std::invalid_argument("Unaligned data");
    }

    tStream.writeTStream(reinterpret_cast<const char*>(data.data()), data.size(),  true);
    tStream.setSeekPos(ogSeek);

}

std::optional<std::vector<u8>> BinarySerializer::getFormattedData(bool autoAlign) {
    if (bufferData.empty()) {
        return std::nullopt;
    }
   
    size_t alignSize = 0;

    if (autoAlign) {
        const auto& vSize = bufferData.size();
        if (!isDataSizeAligned(vSize)) {
            alignSize = getByteAlignment(vSize) - vSize;
            dbgprintf("--Unaligned data serialized\nog:%luB new: %luB\n", vSize, alignSize+vSize);
        }
    }

    bufferData.insert(bufferData.end(), alignSize, 0);
    if (!expectAlignedDataOrDie(bufferData.size())) {
            return std::nullopt;
    }
    dbgprintf("--Serializing data size %luB\n\n", bufferData.size());
   // bool st = tStream->writeTStream(reinterpret_cast<const char*>(bufferData.data()), bufferData.size(),  true);
    
    return bufferData;

    
    

}


//todo use ReadLeStream()
