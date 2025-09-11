#pragma once
#include "libtrident.h"
#include "ioflags.h"
#include <fstream>
#include <string>
#include <memory>
#include "trderr.h"
#include "fstreaminfo.h"



#ifdef IS_BIG_ENDIAN

#define cpuToBE16(val) (val)

#define beToCPU16(val) (val)

#define cpuToLE16(val) swapEndian16(val)

#define leToCPU16(val) swapEndian16(val)

#else

#define cpuToBE16(val) swapEndian16(val)

#define beToCPU16(val) swapEndian16(val)

#define cpuToLE16(val) (val)

#define leToCPU16(val) (val)

#endif






//todo add most basic IO function here
namespace LibTrident::PkgIO {
   namespace FileOperations {
            std::string GetFileDir(const std::string& file);
            std::streamsize GetFstreamSize(std::shared_ptr<std::fstream> fs);
            LibTrident::LTSTATUS::LTSTATUS FileOrDirExists(const std::string& path, bool file);
            ///The following 2 functions format the buffer and WriteHeader it as Little endian
            //does not increment fSize
            bool WriteHeaderLeStream(std::shared_ptr<std::fstream> stream,const char* data, std::streamsize size);
            //increments fSize by bytes written by default, if updating alReadHeadery written variable INCREMENT MUST BE FALSE
            bool WriteHeaderLeStream(FstreamInfo::TRDFstreamObject& info, const char* data, std::streamsize size,
                                                                                                        bool increment=true);
                
                
            bool ReadHeaderLeStream(std::shared_ptr<std::fstream> stream, char* s, std::streamsize size);
            bool ReadHeaderLeStream(FstreamInfo::TRDFstreamObject& info, char* s, std::streamsize size);
    };
   

    class Descriptor {
        public:
            //beg=true mean beginning of section, beg=false end of section
            virtual bool WriteHeaderDescriptorSUID(bool beg) = 0;
            virtual bool ReadHeaderDescriptorSUID(bool beg) = 0;
            virtual u32  GenerateCRC() = 0;
            //finds first occurence of token in stream using Boyer Moore Horsepool
            static std::streampos FindDescriptorToken(std::string uuid);
    };

};


