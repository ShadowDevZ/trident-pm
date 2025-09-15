#include "fstreaminfo.h"
//#include "fileOperations.h"
#include "ioflags.h"
#include "pkgio.h"

using namespace LibTrident;
using namespace PkgIO;
using namespace LibTrident::FstreamInfo;
bool FstreamInfo::TrdFstreamInfo::CheckFileStreamInfo() {
    //check if pointer was allocated usiong OpenPkg()
    if (!xfInfo.hFile) {
        return false;
    }
    Err::Code status = ValidateRemoteFileStreamInfo(xfInfo);
    if (!TrdFstreamInfo::IsOpen()) {
        return false;
    }
    if (status == Err::Code::SUCCESS) {
        e.Success();
        return true;
    }
    e.SetError(status);
    return false;
}
LibTrident::Err::Code  FstreamInfo::TrdFstreamInfo::StreamRemoteIsOpen(const TRDFstreamObject& info) {
    if (info.fileFlags & LibTrident::IOFLAGS::_I_IO_INVCLOSED) {
        return Err::Code::FOPEN;
    }
    if (!info.hFile || ! info.hFile->is_open() || (!info.fileOpened)) {
    
        return Err::Code::FOPEN;;
    }

    return Err::Code::SUCCESS;
}
bool FstreamInfo::TrdFstreamInfo::IsOpen() {
    
    Err::Code status = StreamRemoteIsOpen(xfInfo);
    e.SetError(status);
    if (status == Err::Code::SUCCESS) {
        return true;
    }
    return false;
}

Err::Code TrdFstreamInfo::CloseRemoteStream(TRDFstreamObject& info) {
    info.fileFlags |= LibTrident::IOFLAGS::_I_IO_INVCLOSED;
    return Err::Code::SUCCESS;
} 
bool FstreamInfo::TrdFstreamInfo::CloseStream() {
    Err::Code status = CloseRemoteStream(xfInfo);
    e.SetError(status);
    if (status == Err::Code::SUCCESS) {
        return true;
    }
    return false;
}


Err::Code FstreamInfo::TrdFstreamInfo::ValidateRemoteFileStreamInfo(const TRDFstreamObject& info) {
   
   /// Err::Code dirStatus = FileOperations::FileOrDirExists(info.dirPath, false);
   // Err::Code fileStatus = FileOperations::FileOrDirExists(info.name, true);

   /*We no longer have to check because we have thrown exception
    if (!std::filesystem::exists(info.dirPath)) {
        dbgprintf("Error: ValidateRemoteFileStreamInfo() nodir\n");
        return Err::Code::NOTDIR;
    }
    if (!std::filesystem::exists(info.name)) {
         dbgprintf("Error: ValidateRemoteFileStreamInfo() nofile\n");
        return Err::Code::NOTFILE;
    }
    */
    if (StreamRemoteIsOpen(info) != Err::Code::SUCCESS) {
         dbgprintf("Error: ValidateRemoteFileStreamInfo() fopen\n");
        return Err::Code::FOPEN;
    }
    if (info.fileFlags == 0) {
        dbgprintf("Error: ValidateRemoteFileStreamInfo() access\n");
        return Err::Code::ACCESS;
    }
    return Err::Code::SUCCESS;
}

bool FstreamInfo::TrdFstreamInfo::SetFileStreamInfo(const TRDFstreamObject& info) {
    
    Err::Code status = ValidateRemoteFileStreamInfo(info);
    if (status != Err::Code::SUCCESS) {
        e.SetError(status);
        return false;
    }

    e.Success();
    xfInfo = info;
    return true;
}


std::optional<std::shared_ptr<FstreamInfo::TrdFstreamInfo>> FstreamInfo::TrdFstreamInfo::GetFstreamContent(std::weak_ptr<FstreamInfo::TrdFstreamInfo> weakFstr) {
   
    auto fstrInfo = weakFstr.lock();
    
    if (!fstrInfo) {
       
        return std::nullopt;
    }
/*
    if (!fstrInfo->CheckFileStreamInfo()) {
        return {fstrInfo->e.GetError(), fstr};
        
    }
  */
    return fstrInfo;
}