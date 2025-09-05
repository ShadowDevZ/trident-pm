#pragma once
#include "libtrident.h"
#include "ioflags.h"
#include <fstream>
#include <string>
#include <memory>
#include "trderr.h"


//todo add most basic IO function here
namespace LibTrident::PkgIO {
   class FileOperations {
        public:
            static std::string GetFileDir(const std::string& file);
            static std::streamsize GetFstreamSize(std::shared_ptr<std::fstream> fs);
            static std::streamsize GetFstreamSize(std::fstream& fs);
            static LibTrident::LTSTATUS::LTSTATUS FileOrDirExists(const std::string& path, bool file);
    };
   

    class Descriptor {
        public:
            virtual bool WriteDescriptorUUID() = 0;
            virtual bool ReadDescriptorUUID() = 0;
            virtual u32  GenerateCRC() = 0;
            //checks if the uuid format and length is correct
            static bool ValidateUUID(std::string uuid);
            //finds first occurence of token in stream using Boyer Moore Horsepool
            static std::streampos FindDescriptorToken(std::string uuid);
    };

};


