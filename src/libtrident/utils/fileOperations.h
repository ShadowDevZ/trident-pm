#pragma once
#include <fstream>
#include <string>
#include <memory>
#include "trderr.h"

namespace Utilities {
    class FileOperations {
        public:
            static std::string GetFileDir(const std::string& file);
            static std::streamsize GetFstreamSize(std::shared_ptr<std::fstream> fs);
            static std::streamsize GetFstreamSize(std::fstream& fs);
            static LibTrident::LTSTATUS::LTSTATUS FileOrDirExists(const std::string& path, bool file);
    };
};