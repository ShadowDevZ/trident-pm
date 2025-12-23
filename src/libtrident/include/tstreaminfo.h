#pragma once
#include "trderr.h"
#include <memory>
#include "datatypes.h"
#include <sys/stat.h>
#include <optional>
#include "ioflags.h"
#include <filesystem>
#include <expected>
//#include "ioflags.h"
namespace LibTrident::Tstream {

typedef struct {
    //todo enforce PATHMAX and use const char* to avoid unnecessary memory allocation
    std::filesystem::path absolutePath;
    std::shared_ptr<std::fstream> hFile;
    //does not represent actual file size, but sizeof(whole file - header), not utilized yet
    std::streamsize fSize;
    IOFLAGS::TRDAccessModel acccessModel;
    //for future use, atime
    struct stat64 fileStat;
    //struct stat64 fileStat;
    bool fileOpened;

}TRDFstreamObject;



class TStreamInfo{
private:
    TRDFstreamObject xfInfo;
public:
    

    
   
    TRDFstreamObject& GetFstreamObject() {
        return xfInfo;
        
    }
    const TRDFstreamObject& GetFstreamObject() const {
        return xfInfo;
    }
    //throws std::runtime on failure
    void SetFileStreamInfo(const TRDFstreamObject& info);
    

    //checks if each field is correctly set
    std::expected<void, Err::TrdError> CheckFileStreamInfo() const;
    std::expected<void, Err::TrdError> CloseStream();
    //checks if the stream is only MARKED as open, data may be missing or corrupted
    std::expected<void, Err::TrdError> IsOpen() const;
    //throws std::iosbase::failure on exception
    void SetSeekPos(u64 pos, std::ios_base::seekdir seek=std::ios::beg);
    //throws std::iosbase::failure on exception
    i64 GetSeekPos() const;
    
    
    template <typename T>
    void WriteTStream(const T& t, u64 size, bool increment=true) {
        WriteTStream(reinterpret_cast<const char*>(&t), size, increment);
    }
    template <typename T>
    void WriteTStream(T& t, bool increment=true) {
        WriteTStream(t, sizeof(t), increment);
    }
    //throws std::invalid_argument runtime error or anything by FileOperations::WriteLeData
    void WriteTStream(const char* data, u64 size, bool increment=true);
    void WritePadding(u16 size, int value=0, bool increment=true);
    
    
    
    template <typename T>
    void ReadTStream(T& t, u64 size) const {
        ReadTStream(reinterpret_cast<char*>(&t), size);
    }
    
    template <typename T>
    void ReadTStream(T& t) const{
        ReadTStream(t, sizeof(t));
    }

    //throws std::invalid_argument runtime error or anything by FileOperations::ReadLeData
    void ReadTStream(char* s, u64 size) const;

 
};



};