#include "fstreaminfo.h"
//#include "fileOperations.h"
#include "ioflags.h"
#include "pkgio.h"
using namespace LibTrident;
using namespace PkgIO;

bool FstreamInfo::TrdFstreamInfo::CheckFileStreamInfo() {
    LTSTATUS::LTSTATUS status = ValidateFileStreamInfo(*xfInfo);
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
LibTrident::LTSTATUS::LTSTATUS  FstreamInfo::TrdFstreamInfo::StreamIsOpen(const TRDFilStreameInfo& info) {
    if (info.fileFlags & LibTrident::IOFLAGS::_I_IO_INVCLOSED) {
        return LTSTATUS::FOPEN;
    }
    if (!info.hFile || ! info.hFile->is_open()) {
    
        return LTSTATUS::FOPEN;;
    }

    return LTSTATUS::SUCCESS;
}
bool FstreamInfo::TrdFstreamInfo::IsOpen() {
    
    LTSTATUS::LTSTATUS status = StreamIsOpen(*xfInfo);
    e.SetError(status);
    if (status == LTSTATUS::SUCCESS) {
        return true;
    }
    return false;
} 
bool FstreamInfo::TrdFstreamInfo::CloseStream() {
    xfInfo->fileFlags |= LibTrident::IOFLAGS::_I_IO_INVCLOSED;
    //bool for future use
    return true;
}
LTSTATUS::LTSTATUS FstreamInfo::TrdFstreamInfo::ValidateFileStreamInfo(TRDFilStreameInfo& info) {
   
    LTSTATUS::LTSTATUS dirStatus = FileOperations::FileOperations::FileOrDirExists(info.dirPath, false);
    LTSTATUS::LTSTATUS fileStatus = FileOperations::FileOperations::FileOrDirExists(info.name, true);
    if (dirStatus != LTSTATUS::SUCCESS) {
        return LTSTATUS::NOTDIR;
    }
    if (fileStatus != LTSTATUS::SUCCESS) {
        return LTSTATUS::NOTFILE;
    }
    if (StreamIsOpen(info) != LTSTATUS::SUCCESS) {
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