#include "libtrident.h"
#include <iostream>
#include "ioflags.h"
#include <fstream>
#include <sys/stat.h>
#include <stdexcept>


#include "trheader.h"
#include "sdesc.h"
using namespace Trd;
using namespace Trd::Impl;

TrFileHeader TrPkg::header() {
    return TrFileHeader(*this);
}
TrSectionDescriptor TrPkg::sectionDescriptor() {
    return TrSectionDescriptor(*this);
}

void Trd::TrPkg::closePkg() {
    //we do not perform any checks so RAII can take care of it
    TRDFstreamObject& closeInfo =  fstrInfo.getFstreamObject();
    closeInfo.absolutePath.clear();
    closeInfo.fileOpened = false;
    fstrInfo.closeStream();
    if (closeInfo.hFile && closeInfo.hFile->is_open()) {
        closeInfo.hFile->close();
    }
    dbgprintf("Stream closed\n");
}


void Trd::TrPkg::openPackage(const std::filesystem::path& path,const TRDAccessModel& accessModel) {
    if (fstrInfo.getFstreamObject().fileOpened) {
        throw std::runtime_error("Package was already opened using current instance");
    }
    
    //std::ios::openmode openMode = IOFLAGS::IOFlags2FsBase(openFlags);
    auto optOpenMode = Impl::translateAccessModel(accessModel);
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
    
    
    
   
    //We are creating copy instead of simply moving is because if error occurs the original stream must remain unchanged
    TRDFstreamObject fInfo {};
    
    fInfo.acccessModel = accessModel;
    fInfo.checksumSize = std::filesystem::file_size(path);

   // fInfo.hFile->seekg(0, std::ios::beg); 
    
    dbgprintf("Seek offset %lu\n", static_cast<u64>(fsPkg->tellg())); 
    dbgprintf("File size %luB\n", static_cast<u64>(fInfo.checksumSize)); 
   
    fInfo.hFile = fsPkg;
    
    fInfo.absolutePath = absolutePath;
    dbgprintf("name: %s\n", fInfo.absolutePath.filename().c_str()); 
    dbgprintf("dir: %s\n", fInfo.absolutePath.parent_path().c_str()); 
    auto fileStat = Trd::Impl::statFile(path);
    if (!fileStat.has_value()) {
        throw std::runtime_error("failed to stat() file");
    }
    fInfo.pStat = fileStat.value();
    
    fInfo.fileOpened = true;
    fstrInfo.setFileStreamInfo(fInfo);
    
}
//ClosePkg, reset context, filestream close fd