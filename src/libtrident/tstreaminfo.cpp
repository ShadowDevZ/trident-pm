#include "tstreaminfo.h"
//#include "fileOperations.h"
#include "ioflags.h"
#include "pkgio.h"
#include <cstring>
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

void Tstream::TStreamInfo::SetFileStreamInfo(const TRDFstreamObject& info) {
    
    e.AssertOkOrDie("StreamRemoteIsOpen() failed, err: ", StreamRemoteIsOpen(info));
  
    xfInfo = info;
    e.Success();
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



void TStreamInfo::WriteTStream(const char* data, u64 size, bool increment) {
    if (!CheckFileStreamInfo())  {
        throw std::runtime_error("WriteLeStream(validate) Failed");
    }
    //for compatibility across different CPUS and to improve performance on x86/64
    if (!BinarySerializer::ExpectAlignedDataOrDie(size)) {
        e.SetError(Err::Code::ALIGNMENT);
    }
    if (size == 0) {
        throw std::invalid_argument("Size was 0");
    }
    
    //throws exception on failure, no need to check
    FileOperations::WriteLeData(xfInfo.hFile, data, size);

    if (increment) {
        xfInfo.fSize += size;
    }
}
void TStreamInfo::ReadTStream(char* s, u64 size) {
    if (size == 0) {
        throw std::invalid_argument("Size was 0");
    }
    if (!CheckFileStreamInfo())  {
        throw std::runtime_error("WriteLeStream(validate) Failed");
    }
    if (!BinarySerializer::ExpectAlignedDataOrDie(size)) {
        e.SetError(Err::Code::ALIGNMENT);
    }

    FileOperations::ReadLeData(xfInfo.hFile, s, size);
}


void TStreamInfo::ISetSeekPos(bool read, u64 pos, std::ios_base::seekdir seekd) {
    if (read) {
        xfInfo.hFile->seekg(pos, seekd);
    }
    else {
        xfInfo.hFile->seekp(pos, seekd);
    }
    if (!xfInfo.hFile) {
        e.SetError(Err::Code::FSEEK);
        //i was actually thinking if exceptions are necessary here but given that the user could set invalid offset
        //and this could invalidate the whole program means we would have to check seekpos in every function 
        throw std::ios_base::failure("seekg() failure");
    }
    e.Success();
   


}
i64 TStreamInfo::IGetSeekPos(bool read) {
    i64 pos = -1;
    if (read) {
        pos =  xfInfo.hFile->tellg();
    }
    else {
        pos =  xfInfo.hFile->tellp();
    }
    if (pos == -1 || !xfInfo.hFile) {
        e.SetError(Err::Code::FSEEK);
        throw std::ios_base::failure("tellg() failed");
    }
    e.Success();
    return pos;
}

void TStreamInfo::WritePadding(u16 size, int value, bool increment) {
    char padding[size];
    std::memset(padding, value, sizeof(padding));
    WriteTStream(padding, sizeof(padding), increment);
}

