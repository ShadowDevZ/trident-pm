#include "tstreaminfo.h"
//#include "fileOperations.h"
#include "ioflags.h"
#include "pkgio.h"

using namespace LibTrident;
using namespace PkgIO;
using namespace LibTrident::Tstream;
bool Tstream::TStreamInfo::CheckFileStreamInfo() {
    //check if pointer was allocated usiong OpenPkg()
    if (!xfInfo.hFile) {
        e.SetError(Err::Code::FOPEN);
        return false;
    }
    
    if (!TStreamInfo::IsOpen()) {
        e.SetError(Err::Code::FOPEN);
        return false;
    }
    if (xfInfo.fileFlags == 0) {
        e.SetError(Err::Code::ACCESS);
        return false;
    }
    e.Success();
    return true;
}

Err::Code StreamRemoteIsOpen(const TRDFstreamObject& info) {
    if (info.fileFlags & LibTrident::IOFLAGS::_I_IO_INVCLOSED) {
        return Err::Code::FOPEN;
    }
    if (info.fileFlags == 0) {
        return Err::Code::ACCESS;
    }
    if (!info.hFile || ! info.hFile->is_open() || (!info.fileOpened)) {
    
        return Err::Code::FOPEN;;
    }

    return Err::Code::SUCCESS;
}



bool Tstream::TStreamInfo::IsOpen() {
    Err::Code status = StreamRemoteIsOpen(xfInfo);
    e.SetError(status);
    if (!e.IsOk()) {
        return false;
    } 
    return true;
}
/*
Err::Code TStreamInfo::CloseRemoteStream(TRDFstreamObject& info) {
    info.fileFlags |= LibTrident::IOFLAGS::_I_IO_INVCLOSED;
    return Err::Code::SUCCESS;
} 
*/
bool Tstream::TStreamInfo::CloseStream() {
    xfInfo.fileFlags |= LibTrident::IOFLAGS::_I_IO_INVCLOSED;
    return true;
   
}

/*
Err::Code Tstream::TStreamInfo::ValidateRemoteFileStreamInfo(const TRDFstreamObject& info) {
   
   /// Err::Code dirStatus = FileOperations::FileOrDirExists(info.dirPath, false);
   // Err::Code fileStatus = FileOperations::FileOrDirExists(info.name, true);

   **We no longer have to check because we have thrown exception
    if (!std::filesystem::exists(info.dirPath)) {
        dbgprintf("Error: ValidateRemoteFileStreamInfo() nodir\n");
        return Err::Code::NOTDIR;
    }
    if (!std::filesystem::exists(info.name)) {
         dbgprintf("Error: ValidateRemoteFileStreamInfo() nofile\n");
        return Err::Code::NOTFILE;
    }
    **
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
*/
bool Tstream::TStreamInfo::SetFileStreamInfo(const TRDFstreamObject& info) {
    Err::Code status = StreamRemoteIsOpen(info);
    e.SetError(status);
    if (!e.IsOk()) {
        return false;
    }
    xfInfo = info;
    return true;
}


std::optional<std::shared_ptr<Tstream::TStreamInfo>> Tstream::TStreamInfo::GetFstreamContent(std::weak_ptr<Tstream::TStreamInfo> weakFstr) {
   
    auto fstrInfo = weakFstr.lock();
    
    if (!fstrInfo) {
       
        return std::nullopt;
    }

    if (!fstrInfo->CheckFileStreamInfo()) {
        return std::nullopt;
        
    }
      return fstrInfo;
}

bool TStreamInfo::WriteTStream(const char* data, u64 size, bool increment) {
    if (!CheckFileStreamInfo())  {
        dbgprintf("Error: WriteLeStream(validate) Failed\n");
        return false;
    }
   
    bool st = FileOperations::WriteLeData(xfInfo.hFile, data, size);
    if (st && increment) {
        xfInfo.fSize += size;
    }
    return st;
}
bool TStreamInfo::ReadTStream(char* s, u64 size) {
    if (!CheckFileStreamInfo())  {
        dbgprintf("Error: ReadLeStream(validate) Failed\n");
        return false;
    }
    return FileOperations::ReadLeData(xfInfo.hFile, s, size);
}


bool TStreamInfo::ISetSeekPos(bool read, u64 pos, std::ios_base::seekdir seekd) {
    if (read) {
        xfInfo.hFile->seekg(pos, seekd);
    }
    else {
        xfInfo.hFile->seekp(pos, seekd);
    }
    if (!xfInfo.hFile) {
        e.SetError(Err::Code::FSEEK);
        return false;
    }
    e.Success();
    return true;


}
u64 TStreamInfo::IGetSeekPos(bool read) {
    u64 pos = -1;
    if (read) {
        pos =  xfInfo.hFile->tellg();
    }
    else {
        pos =  xfInfo.hFile->tellp();
    }
    if (pos == -1 || !xfInfo.hFile) {
        e.SetError(Err::Code::FSEEK);
        return -1;
    }
    e.Success();
    return pos;
}