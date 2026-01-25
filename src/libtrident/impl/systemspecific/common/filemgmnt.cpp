#include "include/filemgmnt.h"
#include "../platform/Linux/include/linuxSpecific.h"
#include <filesystem>
using namespace LibTrident;

namespace fs = std::filesystem;
std::expected<std::filesystem::path, LibTrident::Err::TrdError> Impl::createTemporaryFile() {
    #if defined(__linux__) || defined(__unix__) 
        return LinuxSpecific::createTemporaryFile();
    #elif defined(_WIN32_)
        #error "Function not implemented"
    #else
        #error "Function not implemented"
    #endif

}
std::expected<LibTrident::Impl::AuxiliaryStat, LibTrident::Err::TrdError> LibTrident::Impl::getAuxiliaryStat(const std::filesystem::path& path) {
    #if defined(__linux__) || defined(__unix__) 
        return LinuxSpecific::getAuxiliaryStat(path);
    #elif defined(_WIN32_)
        #error "Function not implemented"
    #else
        #error "Function not implemented"
    #endif
}

std::expected<LibTrident::Impl::PortableStat, LibTrident::Err::TrdError> LibTrident::Impl::statObject(const std::filesystem::path& file) {
    if (!fs::exists(file)) {
        return std::unexpected(Err::TrdError{Err::Code::FileMissing});
    }
    auto status = fs::symlink_status(file);
    
  
    
    Impl::PortableStat pStat {};
    pStat.fileType = status.type();
    pStat.permissions = status.permissions();
    //checking because folders and other devices dont have size
    if (fs::is_regular_file(status)) {
        pStat.fileSize = fs::file_size(file);
    } 
    //std::filesystem only provides mtime, so get everything using stat()
    //the additional info is purely optional
    auto aux = Impl::getAuxiliaryStat(file);
    if (aux.has_value()) {
        pStat.auxiliary = aux.value();
    }
   
    return pStat;
   

    
}
