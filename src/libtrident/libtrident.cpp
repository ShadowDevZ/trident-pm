#include "libtrident.h"
#include <iostream>
#include "ioflags.h"
#include <fstream>
#include <sys/stat.h>
#include <stdexcept>
#include "trheader.h"
using namespace LibTrident;
using namespace PkgIO;

TRDPkgHeader TrPkg::header() {
    return TRDPkgHeader(*this);
}

void LibTrident::TrPkg::ClosePkg() {
    //we do not perform any checks so RAII can take care of it
    Tstream::TRDFstreamObject& closeInfo =  fstrInfo.GetFstreamObject();
    closeInfo.absolutePath.clear();
    closeInfo.fileOpened = false;
    fstrInfo.CloseStream();
    if (closeInfo.hFile && closeInfo.hFile->is_open()) {
        closeInfo.hFile->close();
    }
    dbgprintf("Stream closed\n");
}


void LibTrident::TrPkg::OpenPackage(const std::filesystem::path& path,const IOFLAGS::TRDAccessModel& accessModel) {
    if (fstrInfo.GetFstreamObject().fileOpened) {
        throw std::runtime_error("Package was already opened using current instance");
    }
    
    //std::ios::openmode openMode = IOFLAGS::IOFlags2FsBase(openFlags);
    auto optOpenMode = IOFLAGS::TranslateAccessModel(accessModel);
    if (!optOpenMode.has_value()) {
        throw std::invalid_argument("Incorrect access model used");
    }
    std::ios::openmode openMode = optOpenMode.value();
    
    
    const std::filesystem::path& absolutePath = std::filesystem::absolute(path);
    
    const std::filesystem::path& parentDir = absolutePath.parent_path();
    
    if (!std::filesystem::exists(parentDir)) {
        throw std::filesystem::filesystem_error("Parent directory does not exist", std::error_code());
   }
    
    //the user doesnt need to specify 
    openMode |= std::ios::binary;
  
    std::shared_ptr<std::fstream> fsPkg = std::make_shared<std::fstream>(path, openMode);
    if (!fsPkg || !fsPkg->is_open()) {
        throw std::filesystem::filesystem_error("Failed to obtain file handle", std::error_code());
    }
    
    
    std::streampos fileSize = FileOperations::GetFstreamSize(fsPkg);
    if (fileSize == -1) {
        throw std::runtime_error("Failed to determine the file size");
    }
    //We are creating copy instead of simply moving is because if error occurs the original stream must remain unchanged
    Tstream::TRDFstreamObject fInfo;
    
    fInfo.acccessModel = accessModel;
    fInfo.fSize = fileSize;

   // fInfo.hFile->seekg(0, std::ios::beg); 
    
    dbgprintf("Seek offset %lu\n", static_cast<u64>(fsPkg->tellg())); 
    dbgprintf("File size %luB\n", static_cast<u64>(fInfo.fSize)); 
   
    fInfo.hFile = fsPkg;
    
    fInfo.fileOpened = true;
    fInfo.absolutePath = absolutePath;
    dbgprintf("name: %s\n", fInfo.absolutePath.filename().c_str()); 
    dbgprintf("dir: %s\n", fInfo.absolutePath.parent_path().c_str()); 
    fInfo.fileStat = FileOperations::GetFileStats(path);

    fstrInfo.SetFileStreamInfo(fInfo);

}
//ClosePkg, reset context, filestream close fd