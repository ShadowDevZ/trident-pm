#include "sdesc.h"
#include "sdescid.h"
#include "trheader.h"
//#include "pkgio.h"
//#include "suid.h"
#include "trdconsts.h"
#include "tstreaminfo.h"
#include <cstring>
#include "trderr.h"
#include <zlib.h>
using namespace LibTrident::SectionDescriptor;
using namespace LibTrident::Header;
//using namespace LibTrident::UID;
using namespace LibTrident;

u32 IGenerateChecksum(const TRD_SD& sd);


//starting location of SD table without BEG/END token

//difference between these and raw functions is that Raw function point to the start of GUID whilst these point to actual data
//less error checking
foffset_t TRDSecDesc::GetSDAddress() noexcept { 
    //todo actually find the TUID inside the stream and get its position to check presence start
   return TRDSdToken::GetOptRawSDStart() + LibTrident::Consts::SUID::SUID_MAX_LENGTH;
}
foffset_t TRDSecDesc::GetSDEnd()  noexcept{
    return GetSDAddress() + LibTrident::Consts::SD::TRD_SECTIONSD_SIZE;
}
//todo for normal write lookup the SUID using bmh algo from uid.cpp in future
bool TRDSecDesc::WriteBlankSD() {
    //just in case there was some garbage before
    auto haveCtx = Tstream::TStreamInfo::GetFstreamContent(wFstr);
    if (!haveCtx.has_value()) {
        e.SetError(Err::Code::IREF_EXPIRED);
        return false;
    }
    auto sdStream = haveCtx.value();
    //to use std::filln we would have to write iterator
    std::memset(&secDescInternal, 0, sizeof(secDescInternal));
    //write SUID prologue
    TRDSdToken sdToken(sdStream);
    bool begSuidOk = sdToken.WriteDescriptorSUID();
    e.SetError(sdToken.e.GetError());
    if (!begSuidOk) {
        return false;
    }
    return IWriteSD(true);
}


 bool TRDSecDesc::IWriteSD(bool blankWrite) {
    //we do not validate empty TRD_SD
    if (!blankWrite && !IValidateSDContent(secDescInternal)) {
        return false;
    }
    auto haveCtx = Tstream::TStreamInfo::GetFstreamContent(wFstr);
    if (!haveCtx.has_value()) {
        e.SetError(Err::Code::IREF_EXPIRED);
        return false;
    }
    auto sdStream = haveCtx.value();
    if (!sdStream->CheckFileStreamInfo()) {
        e.SetError(Err::Code::NULL_OBJ);
        return false;
    }
    if (!IRwAccessible(sdStream)) {
        return false;
    }
    
   // auto& fstrInfo = sdStream->GetFstreamObject();
   // auto& fstrStream = sdStream->GetFstreamObject().hFile;
    
    sdStream->SetSeekPosW(GetSDAddress());
    
    //const char* hdrContent = reinterpret_cast<const char*>(&secDescInternal);
    sdStream->WriteTStream<TRD_SD>(secDescInternal);
    if (!sdStream->e.IsOk()) {
        e.SetError(Err::Code::IO_WRITE);
        return false;
    }


    e.Success();
    return true;
}
//checks if we have header first
bool TRDSecDesc::IRwAccessible(){
    //todo check
   return IRwAccessible(wFstr);
}
bool TRDSecDesc::IRwAccessible(std::weak_ptr<LibTrident::Tstream::TStreamInfo> fstr) {
    //todo check
    Err::Code hdrStatus = TRDPkgHeader::IsHeaderPresent(fstr);
    if (hdrStatus != Err::Code::SUCCESS) {
        e.SetError(hdrStatus);
        return false;
    }
    //todo find if SUID tag is present
    e.Success();
    return true;
}

bool TRDSecDesc::UpdateSD(const TRD_SD_UPDATEFIELD& sd, bool autoWrite) {
    TRD_SD updateSd = secDescInternal;

    updateSd.tblCount = sd.tblCount;
    updateSd.tblDynamicOffset = sd.tblDynamicOffset;
    updateSd.tblRegistryOffset = sd.tblRegistryOffset;
    updateSd.crc = IGenerateChecksum(updateSd);
    if (!IValidateSDContent(updateSd)) {
        return false;
    }
    secDescInternal = updateSd;
    
    if (autoWrite) {
        return IWriteSD();
    }
    return true;
}

/*
if we are going to have to perform random access we probably should use mmap() syscall, in that 
case we probably should also in PkgIO create custom std::ostream clonse for memory mapped file, mimicking the
seek/tell functions, this will however make it more os dependent and we are going to need multiple os specific function
prototypes. There is also problem that when we write to the file and size changes we have to again call mmap() because
the kernel won't update the size automatically resulting in SIGBUS, this becomes problematic. Perhaps in future we could
utilize header only cross platform library like https://github.com/vimpunk/mio
*/
bool TRDSecDesc::IValidateSDContent(const TRD_SD& sd) {
    if (!IChecksumValid(sd.crc, sd)) {
        e.SetError(Err::Code::CHKSUM);
    }
    if (sd._reserved0 != 0) {
        e.SetError(Err::Code::RESV_VIOLATION);
        return false;
    }
    //todo check crc and fields, this will be done when we actually have dynamic section table
    e.Success();
    return true;
}
bool TRDSecDesc::IValidateTblAddr(const TRD_SD& sd) {
    if (sd.tblCount == 0 || sd.tblDynamicOffset == 0 || sd.tblRegistryOffset == 0) {
        //todo actually check each offset
        e.SetError(Err::Code::COPYOBJ);
        return false;
    }
    e.Success();
    return true;

}
u32 IGenerateChecksum(const TRD_SD& sd) {
    #define _LOCAL_CRC(crc,x) crc32(((crc)), reinterpret_cast<const Bytef*>(&(x)), sizeof((x)))
    u32 crc = ::crc32(0, Z_NULL, 0);
    crc = _LOCAL_CRC(crc, sd.tblCount);
    crc = _LOCAL_CRC(crc, sd.tblDynamicOffset);
    crc = _LOCAL_CRC(crc, sd.tblRegistryOffset);
    //do not generate for crc, it doesn't contain valid value yet
    return crc;
}
bool TRDSecDesc::IChecksumValid(u32 crc, const TRD_SD& sd) {
    u32 genCrc = IGenerateChecksum(sd);
    if ((genCrc != sd.crc) || (crc == 0)) {
        e.SetError(Err::Code::CHKSUM);
        return false;
    } 
    return true;
}
//These functions have to calculate CRC32 unlike the Header UpdateX
bool TRDSecDesc::UpdateSDTblCount(u32 tblCount) {
    secDescInternal.tblCount = tblCount;
    secDescInternal.crc = IGenerateChecksum(secDescInternal);
    return IWriteSD();
}
bool TRDSecDesc::UpdateSDDynOffset(u64 dynOffset) {
    secDescInternal.tblDynamicOffset = dynOffset;
    secDescInternal.crc = IGenerateChecksum(secDescInternal);
    return IWriteSD();
}
bool TRDSecDesc::UpdateSDRegOffset(u64 tregOffset) {
    secDescInternal.tblRegistryOffset = tregOffset;
    secDescInternal.crc = IGenerateChecksum(secDescInternal);
    return IWriteSD();
}

std::optional<TRD_SD> TRDSecDesc::ReadBack() {
    TRD_SD sdDesc { };
    
    auto haveCtx= Tstream::TStreamInfo::GetFstreamContent(wFstr);
    if (!haveCtx.has_value()) {
        e.SetError(Err::Code::IREF_EXPIRED);
        return std::nullopt;
    }
    auto sdStream = haveCtx.value();
    
    
    if (!sdStream->CheckFileStreamInfo()) {
        e.SetError(Err::Code::NULL_OBJ);
        return std::nullopt;
    }
    
    
    i64 originalPosition = sdStream->GetSeekPosR();

    if (!sdStream->e.IsOk() || originalPosition == -1) {
        e.SetError(Err::Code::FSEEK);
        return std::nullopt;
    }
    if (!IRwAccessible(sdStream)) {
        e.SetError(Err::Code::IO_READ);
        return std::nullopt;    
    }

    sdStream->SetSeekPosR(GetSDAddress());
   
    
   // ReadHeaderStream.ReadHeader(reinterpret_cast<char*>(&hdr), GetHeaderByteSize());

    sdStream->ReadTStream<TRD_SD>(sdDesc);
  

    sdStream->SetSeekPosR(originalPosition);

    if (!IValidateSDContent(sdDesc)) {
        return std::nullopt;
    }
    e.Success();
    return sdDesc;
 }
 bool TRDSecDesc::Read() {
    auto optHdr = TRDSecDesc::ReadBack();
    //todo call when implemented IValidateTblAddr
    if (optHdr.has_value() && IValidateSDContent(optHdr.value())) {
        secDescInternal = optHdr.value();
        return true;
    }
    
    return false;
}

bool TRDSecDesc::IsValid() {
    auto have = Tstream::TStreamInfo::GetFstreamContent(wFstr);
    if (!have.has_value()) {
        e.SetError(Err::Code::IREF_EXPIRED);
        return false;
    }
    if (!have.value()->IsOpen()) {
        e.SetError(Err::Code::FOPEN);
        return false;
    }
    /* use when actually properly implemented
    if (!IValidateTblAddr(secDescInternal)) {
        return false;
    }
    */
    return IValidateSDContent(secDescInternal);
}


LibTrident::Err::Code TRDSecDesc::IsSDPresent() {
    return Err::Code::FNNOTIMPL;
}