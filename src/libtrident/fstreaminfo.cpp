#include "fstreaminfo.h"
#include "fileOperations.h"

using namespace LibTrident;

bool FstreamInfo::TrdFstreamInfo::CheckFileStreamInfo() {
    LTSTATUS::LTSTATUS status = ValidateFileStreamInfo(*xfInfo);
    if (status == LTSTATUS::SUCCESS) {
        e.Success();
        return true;
    }
    e.SetError(status);
    return false;
}

LTSTATUS::LTSTATUS FstreamInfo::TrdFstreamInfo::ValidateFileStreamInfo(TRDFilStreameInfo& info) {
   
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
    if (info.fileFlags == 0 || info.seekOffsetRead == -1 || info.seekOffsetWrite == -1) {
        return LTSTATUS::ACCESS;
    }
    return LTSTATUS::SUCCESS;
}

bool FstreamInfo::TrdFstreamInfo::SetFileStreamInfo(TRDFilStreameInfo& info) {
    
    LTSTATUS::LTSTATUS status = ValidateFileStreamInfo(info);
    if (status != LTSTATUS::SUCCESS) {
        e.SetError(status);
        return false;
    }

    e.Success();
    *xfInfo = info;
    return true;
}