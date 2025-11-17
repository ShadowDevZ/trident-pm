#pragma once
#include "trderr.h"
#include <memory>
#include "datatypes.h"
#include <sys/stat.h>
#include <optional>
//#include "ioflags.h"
namespace LibTrident::Tstream {

typedef struct {
    //todo enforce PATHMAX and use const char* to avoid unnecessary memory allocation
    std::string dirPath;
    std::string name;
    std::shared_ptr<std::fstream> hFile;
    //does not represent actual file size, but sizeof(whole file - header), not utilized yet
    std::streamsize fSize;
    IO_OpenFlag fileFlags;
    //for future use, atime
    std::shared_ptr<struct stat64> fileStat;
    //struct stat64 fileStat;
    bool fileOpened;

}TRDFstreamObject;



class TStreamInfo{
private:
    i64 IGetSeekPos(bool read);
    void ISetSeekPos(bool read, u64 pos, std::ios_base::seekdir seek=std::ios::beg);
    TRDFstreamObject xfInfo;
public:
    LibTrident::Err::TridentError e;

    
   
    TRDFstreamObject& GetFstreamObject() {
        return xfInfo;
        
    }
    const TRDFstreamObject& GetFstreamObject() const {
        return xfInfo;
    }
    //throws std::runtime on failure
    void SetFileStreamInfo(const TRDFstreamObject& info);
    

    //checks if each field is correctly set
    bool CheckFileStreamInfo();
    bool CloseStream();
    //checks if the stream is only MARKED as open, data may be missing or corrupted
    bool IsOpen();
     //throws std::iosbase::failure on exception
    inline void SetSeekPosW(i64 pos, std::ios_base::seekdir seekDir=std::ios::beg) {
        ISetSeekPos(true, pos, seekDir);
    }
     //throws std::iosbase::failure on exception
    inline void SetSeekPosR(i64 pos, std::ios_base::seekdir seekDir=std::ios::beg) {
        ISetSeekPos(false, pos, seekDir);
    }
    //throws std::iosbase::failure on exception
    inline i64 GetSeekPosW() {
        return IGetSeekPos(false);
    }
     //throws std::iosbase::failure on exception
    inline i64 GetSeekPosR() {
        return IGetSeekPos(true);
    }

    
    
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
    void ReadTStream(T& t, u64 size) {
        ReadTStream(reinterpret_cast<char*>(&t), size);
    }
    
    template <typename T>
    void ReadTStream(T& t) {
        ReadTStream(t, sizeof(t));
    }

    //throws std::invalid_argument runtime error or anything by FileOperations::ReadLeData
    void ReadTStream(char* s, u64 size);

   static std::optional<std::shared_ptr<Tstream::TStreamInfo>> GetFstreamContent(std::weak_ptr<Tstream::TStreamInfo> weakFstr);
 
};



};