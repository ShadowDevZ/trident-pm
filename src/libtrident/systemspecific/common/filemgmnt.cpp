#include "filemgmnt.h"
#include "../platform/Linux/filemgmnt_linux.hpp"
using namespace LibTrident;
std::optional<std::filesystem::path> LibTrident::SystemSpecific::CreateTemporaryFile() {
    #if defined(__linux__) || defined(__unix__) 
        return LinuxSpecific::CreateTemporaryFile();
    #elif defined(_WIN32_)
        #error "Function not implemented"
    #else
        #error "Function not implemented"
    #endif

}