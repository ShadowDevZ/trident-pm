#include "fstreaminfo.h"
#include "fileOperations.h"
bool LibTrident::FstreamInfo::TrdFstreamInfo::SetFileStreamInfo(std::shared_ptr<TRDFilStreameInfo> info) {
    return SetFileStreamInfo(*info);
}
LibTrident::LTSTATUS::LTSTATUS LibTrident::FstreamInfo::TrdFstreamInfo::ValidateFileStreamInfo(TRDFilStreameInfo& info) {
   
    LTSTATUS::LTSTATUS dirStatus = Utilities::FileOperations::FileOrDirExists(info.dirPath, false);
    LTSTATUS::LTSTATUS fileStatus = Utilities::FileOperations::FileOrDirExists(info.name, true);
    if (dirStatus != LTSTATUS::SUCCESS) {
        return LTSTATUS::NOTDIR;
    }
    if (fileStatus != LTSTATUS::SUCCESS) {
        return LTSTATUS::NOTFILE;
    }
    if (!info.hFile || !info.hFile->is_open()) {
        return LTSTATUS::FOPEN;
    }
    if (info.fileFlags == 0 || info.seekOffset == -1) {
        return LTSTATUS::ACCESS;
    }
    return LTSTATUS::SUCCESS;
}

bool LibTrident::FstreamInfo::TrdFstreamInfo::SetFileStreamInfo(TRDFilStreameInfo& info) {
    
    LTSTATUS::LTSTATUS status = ValidateFileStreamInfo(info);
    if (status != LTSTATUS::SUCCESS) {
        e.SetError(status);
        return false;
    }

    e.Success();
    *xfInfo = info;
    return true;
}