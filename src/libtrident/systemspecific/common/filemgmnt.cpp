#include "include/filemgmnt.h"
#include "../platform/Linux/include/linuxSpecific.h"
#include <filesystem>
using namespace LibTrident;

namespace fs = std::filesystem;
std::expected<std::filesystem::path, LibTrident::Err::TrdError> LibTrident::SystemSpecific::CreateTemporaryFile() {
    #if defined(__linux__) || defined(__unix__) 
        return LinuxSpecific::CreateTemporaryFile();
    #elif defined(_WIN32_)
        #error "Function not implemented"
    #else
        #error "Function not implemented"
    #endif

}
std::expected<LibTrident::PortableTypes::AuxiliaryStat, LibTrident::Err::TrdError> LibTrident::SystemSpecific::GetAuxiliaryStat(const std::filesystem::path& path) {
    #if defined(__linux__) || defined(__unix__) 
        return LinuxSpecific::GetAuxiliaryStat(path);
    #elif defined(_WIN32_)
        #error "Function not implemented"
    #else
        #error "Function not implemented"
    #endif
}

std::expected<LibTrident::PortableTypes::PortableStat, LibTrident::Err::TrdError> LibTrident::SystemSpecific::StatObject(const std::filesystem::path& file) {
    if (!fs::exists(file)) {
        return std::unexpected(Err::TrdError{Err::Code::FileMissing});
    }
    auto status = fs::symlink_status(file);
    
  
    
    PortableTypes::PortableStat pStat {};
    pStat.fileType = status.type();
    pStat.permissions = status.permissions();
    //checking because folders and other devices dont have size
    if (fs::is_regular_file(status)) {
        pStat.fileSize = fs::file_size(file);
    } 
    //std::filesystem only provides mtime, so get everything using stat()
    //the additional info is purely optional
    auto aux = SystemSpecific::GetAuxiliaryStat(file);
    if (aux.has_value()) {
        pStat.auxiliary = aux.value();
    }
   
    return pStat;
   

    
}
