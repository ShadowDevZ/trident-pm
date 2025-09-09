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
    LTSTATUS::LTSTATUS status = ValidateRemoteFileStreamInfo(xfInfo);
    if (!TrdFstreamInfo::IsOpen()) {
        return false;
    }
    if (status == LTSTATUS::SUCCESS) {
        e.Success();
        return true;
    }
    e.SetError(status);
    return false;
}
LibTrident::LTSTATUS::LTSTATUS  FstreamInfo::TrdFstreamInfo::StreamRemoteIsOpen(const TRDFilStreameInfo& info) {
    if (info.fileFlags & LibTrident::IOFLAGS::_I_IO_INVCLOSED) {
        return LTSTATUS::FOPEN;
    }
    if (!info.hFile || ! info.hFile->is_open()) {
    
        return LTSTATUS::FOPEN;;
    }

    return LTSTATUS::SUCCESS;
}
bool FstreamInfo::TrdFstreamInfo::IsOpen() {
    
    LTSTATUS::LTSTATUS status = StreamRemoteIsOpen(xfInfo);
    e.SetError(status);
    if (status == LTSTATUS::SUCCESS) {
        return true;
    }
    return false;
}

LTSTATUS::LTSTATUS TrdFstreamInfo::CloseRemoteStream(TRDFilStreameInfo& info) {
    info.fileFlags |= LibTrident::IOFLAGS::_I_IO_INVCLOSED;
    return LTSTATUS::SUCCESS;
} 
bool FstreamInfo::TrdFstreamInfo::CloseStream() {
    LTSTATUS::LTSTATUS status = CloseRemoteStream(xfInfo);
    e.SetError(status);
    if (status == LTSTATUS::SUCCESS) {
        return true;
    }
    return false;
}


LTSTATUS::LTSTATUS FstreamInfo::TrdFstreamInfo::ValidateRemoteFileStreamInfo(const TRDFilStreameInfo& info) {
   
    LTSTATUS::LTSTATUS dirStatus = FileOperations::FileOrDirExists(info.dirPath, false);
    LTSTATUS::LTSTATUS fileStatus = FileOperations::FileOrDirExists(info.name, true);
    if (dirStatus != LTSTATUS::SUCCESS) {
        dbgprintf("Error: ValidateRemoteFileStreamInfo() nodir\n");
        return LTSTATUS::NOTDIR;
    }
    if (fileStatus != LTSTATUS::SUCCESS) {
         dbgprintf("Error: ValidateRemoteFileStreamInfo() nofile\n");
        return LTSTATUS::NOTFILE;
    }
    if (StreamRemoteIsOpen(info) != LTSTATUS::SUCCESS) {
         dbgprintf("Error: ValidateRemoteFileStreamInfo() fopen\n");
        return LTSTATUS::FOPEN;
    }
    if (info.fileFlags == 0 || info.seekOffsetRead == -1 || info.seekOffsetWrite == -1) {
        dbgprintf("Error: ValidateRemoteFileStreamInfo() access\n");
        return LTSTATUS::ACCESS;
    }
    return LTSTATUS::SUCCESS;
}

bool FstreamInfo::TrdFstreamInfo::SetFileStreamInfo(const TRDFilStreameInfo& info) {
    
    LTSTATUS::LTSTATUS status = ValidateRemoteFileStreamInfo(info);
    if (status != LTSTATUS::SUCCESS) {
        e.SetError(status);
        return false;
    }

    e.Success();
    xfInfo = info;
    return true;
}


std::pair<bool,std::shared_ptr<FstreamInfo::TrdFstreamInfo>> FstreamInfo::TrdFstreamInfo::GetFstreamContent(std::weak_ptr<FstreamInfo::TrdFstreamInfo> weakFstr) {
   
    auto fstrInfo = weakFstr.lock();
    if (!fstrInfo) {
       
        return {false, fstrInfo};
    }
/*
    if (!fstrInfo->CheckFileStreamInfo()) {
        return {fstrInfo->e.GetError(), fstr};
        
    }
  */
    return {true, fstrInfo};
}