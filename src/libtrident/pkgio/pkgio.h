#pragma once
#include "libtrident.h"
#include "ioflags.h"
#include <fstream>
#include <string>
#include <memory>
#include "trderr.h"
#include "tstreaminfo.h"
#include <string_view>
#include <filesystem>



//todo add most basic IO function here
namespace LibTrident::PkgIO {
   namespace FileOperations {
           
            std::streamsize GetFstreamSize(std::shared_ptr<std::fstream> fs);
            
            ///The following 2 functions format the buffer and WriteHeader it as Little endian
            //does not increment fSize
           
            
            bool WriteLeData(std::shared_ptr<std::fstream> stream,const char* data, std::streamsize size);
            //increments fSize by bytes written by default, if updating alReadHeadery written variable INCREMENT MUST BE FALSE
            bool ReadLeData(std::shared_ptr<std::fstream> stream, char* s, std::streamsize size);
             
            bool GetFileStats(const std::filesystem::path& file, struct stat64& statOut);
    };
   

    class Descriptor {
        public:
            virtual ~Descriptor() = default;
            //beg=true mean beginning of section, beg=false end of section
            virtual bool WriteDescriptorSUID() = 0;
            virtual bool ReadDescriptorSUID() = 0;
            virtual u32  GenerateCRC() = 0;
           
    };

};


