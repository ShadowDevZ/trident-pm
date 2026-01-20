#include "tstreaminfo.h"
//#include "fileOperations.h"
#include "ioflags.h"
#include "pkgio.h"
#include <cstring>
using namespace LibTrident;
using namespace PkgIO;
using namespace LibTrident::Tstream;

std::expected<void, Err::TrdError> Tstream::TStreamInfo::checkFileStreamInfo() const {
    //check if pointer was allocated usiong OpenPkg()
    
    if (!TStreamInfo::isOpen() || !xfInfo.hFile) {
        return std::unexpected(Err::TrdError{Err::Code::FileOpenFailure});
    }
    return {};
}

Err::Code streamRemoteIsOpen(const TRDFstreamObject& info) {
    if (info.acccessModel._internal == IOFLAGS::_TrdInternalIO::IoClosed) {
        return Err::Code::FileOpenFailure;
    }
   
    if (!info.hFile || ! info.hFile->is_open() || (!info.fileOpened)) {
    
        return Err::Code::FileOpenFailure;
    }

    return Err::Code::Success;
}



std::expected<void, Err::TrdError> Tstream::TStreamInfo::isOpen() const {
    Err::Code status = streamRemoteIsOpen(xfInfo);
    if (status != Err::Code::Success) {
        return std::unexpected(Err::TrdError{status});
    }
    return {};
}
/*
Err::Code TStreamInfo::CloseRemoteStream(TRDFstreamObject& info) {
    info.fileFlags |= LibTrident::IOFLAGS::_I_IO_INVCLOSED;
    return Err::Code::SUCCESS;
} 
*/
std::expected<void, Err::TrdError> Tstream::TStreamInfo::closeStream() {
    xfInfo.acccessModel._internal = IOFLAGS::_TrdInternalIO::IoClosed;
    return {};
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

void Tstream::TStreamInfo::setFileStreamInfo(const TRDFstreamObject& info) {
    if (streamRemoteIsOpen(info) != Err::Code::Success) {
        throw std::runtime_error("Stream remote is closed");
    }
  
    xfInfo = info;
}





void TStreamInfo::writeTStream(const char* data, u64 size, bool increment) {
    if (!checkFileStreamInfo())  {
        throw std::runtime_error("WriteLeStream(validate) Failed");
    }
    //for compatibility across different CPUS and to improve performance on x86/64
    if (!BinarySerializer::expectAlignedDataOrDie(size)) {
        return;
    }
    if (size == 0) {
        throw std::invalid_argument("Size was 0");
    }
    
    //throws exception on failure, no need to check
    FileOperations::writeLeData(xfInfo.hFile, data, size);

    if (increment) {
        xfInfo.checksumSize += size;
    }
}
void TStreamInfo::readTStream(char* s, u64 size) const {
    if (size == 0) {
        throw std::invalid_argument("Size was 0");
    }
    if (!checkFileStreamInfo())  {
        throw std::runtime_error("WriteLeStream(validate) Failed");
    }
    if (!BinarySerializer::expectAlignedDataOrDie(size)) {
        return;
    }

    FileOperations::readLeData(xfInfo.hFile, s, size);
}


void TStreamInfo::setSeekPos(u64 pos, std::ios_base::seekdir seekd) {
    xfInfo.hFile->seekg(pos, seekd);
    xfInfo.hFile->seekp(pos, seekd);
    if (!xfInfo.hFile) {
        
        //i was actually thinking if exceptions are necessary here but given that the user could set invalid offset
        //and this could invalidate the whole program means we would have to check seekpos in every function 
        throw std::ios_base::failure("seekg() failure");
    }
}
i64 TStreamInfo::getSeekPos() const {
    i64 pos = xfInfo.hFile->tellg();
  
    if (pos == -1 || !xfInfo.hFile) {
        
        throw std::ios_base::failure("tellg() failed");
    }
    return pos;
}

void TStreamInfo::writePadding(u16 size, int value, bool increment) {
    //char padding[size];
    std::vector<char> padding(size);
    std::fill(padding.begin(), padding.end(), value);
   // std::memset(padding, value, sizeof(padding));
    writeTStream(reinterpret_cast<const char*>(padding.data()), padding.size(), increment);
}

