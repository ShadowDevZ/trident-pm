
#include <cstring> 
#include <iostream>
#include "libtrident.h"
#include "suid.h"
#include "sdesc.h"
#include <array>
#include "serdatacommon.h"
#include "trheader.h"
#include <cstdlib>
//TODO THIS FILE SHOULD CONTAIN STATIC_ASSERTIONS
using namespace LibTrident;

using namespace LibTrident::SectionDescriptor;

void PrintBuildTarget() {
#if defined(_LIBTRIDENT_DEBUG)
std::cout << "Target: Debug\n\n";    
#else
std::cout << "Target: Release\n"; 
#endif
}
#include <vector>


#if defined(_LIBTRIDENT_DEBUG)
void print_header(const LibTrident::TRD_HEADER& hdr) {
    dbgprintf("[HEADER_START]\n");
    dbgprintf("\tMagic: ");
    for (auto const& it: hdr.magic) {
        dbgprintf("%X ", it);
    }
    dbgprintf("\n");
    dbgprintf("\tExtened Signature: 0x%X\n",hdr.exSignature);
    auto hdrFmtVal = TRDPkgHeader::headerVersionFormatToString(hdr.fmtVersion);
    if (!hdrFmtVal.has_value()) {
        abort();
    }
    
    dbgprintf("\tVersion Format %s\n", hdrFmtVal.value().c_str());
    dbgprintf("\tCompression: %u\n", hdr.compression);
    dbgprintf("\tBuild flags %u\n", hdr.buildFlags);
    dbgprintf("\tArchitecture %u\n", hdr.architecture);
    dbgprintf("\tChecksum 0x%X\n", hdr.hdrChksum);
    dbgprintf("\tFile length 0x%lXB\n", hdr.fileLen);
    dbgprintf("\tIoControl 0x%X\n", hdr.ioCtrl);
    dbgprintf("[HEADER_END]\n");
}
void print_sd(const LibTrident::SectionDescriptor::TRD_SD& sd) {
    dbgprintf("\n[SD_START]\n");
    dbgprintf("\tCRC: 0x%X\n", sd.crc);
    dbgprintf("\tTblcount: %u\n", sd.tblCount);
    dbgprintf("\tDtbl offset: 0x%lX\n", sd.tblDynamicOffset);
    dbgprintf("\tTreg offset: 0x%lX\n", sd.tblRegistryOffset);
    dbgprintf("\tReserved: %lu\n", sd._reserved0);
    dbgprintf("[SD_END]\n\n");
}

void print_stat(const PortableTypes::PortableStat& ps) {
    u32 perms = static_cast<u32>(ps.permissions) & 0777;
    auto aux = ps.auxiliary.value();

    auto to_time_t = [](auto tp) -> std::time_t {
        return std::chrono::system_clock::to_time_t(tp);
    };
    //ugly debug print, i still dont know how to use std::print, i always get
    //kilometres of unreadable template errors
    //size is expected to be 0 here as we are creating fresh file and data is not written becasue of RAII
    std::cerr << "stat() info\n"  << "Type: " << static_cast<signed char>(ps.fileType) << "\n  Size: " << 
    ps.fileSize.value_or(0) << "\n  Perms: " << std::oct << perms 
    << std::dec << "\n  UID: " << aux.optOwnerUID.value_or(0) << "\n  GID: "
    << aux.optOwnerGID.value_or(0) << "\n  BTIME: " << to_time_t(aux.times.fileCreated.value())
    << "\n  ATIME: " << to_time_t(aux.times.lastAccess) << "\n  CTIME: " 
    << to_time_t(aux.times.lastMetadataChange) << "\n  MTIME: " 
    << to_time_t(aux.times.lastModify) << '\n';  
}

#endif

//test
struct NTC_INFO_TEST : PkgIO::SerializableData{
    uint16_t x = 0; 
    uint32_t y = 0;
    uint16_t z = 0;

    size_t size() const override {
        return PkgIO::BinarySerializer::ElementSize(x,y,z);
    }
    //std::array<uint32_t,2> c{};

//the sum of sizeof of all elements must be properly aligned
    std::optional<std::vector<u8>> serialize() const override {
        PkgIO::BinarySerializer bs;
        bs.addTrivial(x); //2B
        bs.addTrivial(y); //4B
        bs.addTrivial(z); //2B
      //  bs.AddType(c);
       
        return bs.getFormattedData();

    }

    
    //wip idea   void bs::ReadTrivial<T>(const std::vector<u8>& in, const char* outData);
    //if return is false caller throws std::invalid_argument exception
    bool deserialize(const std::vector<u8>& dataIn) override {
        //on error throws exception
        PkgIO::BinarySerializer bs(dataIn);
        size_t xsize = 0;
        xsize += bs.readTrivial<u16>(&x, xsize);
        xsize += bs.readTrivial<u32>(&y, xsize);
        xsize += bs.readTrivial<u16>(&z, xsize);
        
        
        if (xsize != this->size()){
            return false;
        }
      
       // xsize += bs.ReadRaw(&x, sizeof(x), xsize);
      //  xsize += bs.ReadRaw(&y, sizeof(y), xsize);
        //xsize += bs.ReadRaw(&z, sizeof(z), xsize);
        return true;
    }

    

};

template <typename FN, typename EXPR>
requires std::convertible_to<FN, std::string_view>
void tassert(FN fnName, EXPR expr) {
    std::string_view sv(fnName);
    if (!(expr)) {
       std::cerr << "\x1B[31m" << fnName << std::setw(22-sv.size()) << "[fail]" << "\x1B[0m\n";
       std::exit(1);
    }
    else {
        std::cout << "\x1B[32m" << fnName << std::setw(22-sv.size()) << "[ok]" << "\x1B[0m\n";
    }
}

#include "systemspecific/common/include/filemgmnt.h"

//HEAVILY WIP, DO NOT USE THIS BRANCH TESTING ONLY, multiple things are disabled
#warning "Testing branch only, everything is broken here, dont use"

int main(void) {
   // std::cout << SystemSpecific::createTemporaryFile().value() << std::endl;
    PrintBuildTarget();
   
    LibTrident::TrPkg ltTrPkg("./test.tpx", {IOFLAGS::TrdOpenIO::CreateNew,
                                        IOFLAGS::TrdAccessIO::AllAccess,
                                        IOFLAGS::TrdXattrIO::None,
                                        IOFLAGS::_TrdInternalIO::None});
  //  LibTrident::TrPkg ltTrPkg("./test.tpx", IOFLAGS::TrdOpenIO::CreateNew, IOFLAGS::TrdAccessIO::ReadAll);
    //ltTrPkg.OpenPackage("./test.tpx", IOFLAGS::ACCESS_RW | IOFLAGS::CREATE_NEW);



    auto trPkgHdr = ltTrPkg.header();
    
#if defined(_LIBTRIDENT_DEBUG)
  
#endif

    

   
   
    
   
   //std::cout << "CreateHeader()" <<x.Create(BF_PLATF_LINUX, ARCHT_AM64, COMMPRALG_NONE) << std::endl;
    tassert("CreateHeader()",trPkgHdr.create(BF_PLATF_LINUX, ARCHT_AM64, COMMPRALG_NONE));
#if defined(_LIBTRIDENT_DEBUG)
   print_header(trPkgHdr.getHeader());
#endif
// NOLINTNEXTLINE
    tassert("WriteHeader()" , trPkgHdr.write());
  // std::cout << "HeaderRBValid() " << trPkgHdr.IsWrittenHeaderValid() << std::endl;
    tassert("ReadHeader()", trPkgHdr.read());
    
    TRD_HDRFIELD_UPDATE updateField;
    updateField.architecture = ARCHT_AARCH64;
    updateField.buildFlags = BF_PLATF_NT;
    updateField.compression = COMPRALG_GZIP;
    updateField.fmtVersion = TRDPkgHeader::formatHeaderVersion(3,1,2).value();
    
    tassert("UpdateHeader()", trPkgHdr.updateHeader(updateField));
 
    tassert("ValidateHeader()", trPkgHdr.isValid());
#if defined(_LIBTRIDENT_DEBUG)
    print_header(trPkgHdr.getHeader());   
#endif
    tassert("ModifyLen()", trPkgHdr.updateFileLenProp(0xbeefccaa));
    tassert("ModifyIOCTRL()", trPkgHdr.updateIoctrlProp(IOCTRL_DESC_WLOCK));
    tassert("ValidateHeader()", trPkgHdr.isValid());
#if defined(_LIBTRIDENT_DEBUG)
    print_header(trPkgHdr.getHeader());   
#endif
/*
    TRDSecDesc sectionDesc(ltTrPkg.fstrInfo);
    tassert("WriteBlankSD() ", sectionDesc.WriteBlankSD());
     TRD_SD_UPDATEFIELD sdUpdate;
     sdUpdate.tblCount = UINT32_MAX;
     sdUpdate.tblDynamicOffset = UINT64_MAX;
     sdUpdate.tblRegistryOffset = UINT64_MAX;
     tassert("UpdateSD() ", sectionDesc.UpdateSD(sdUpdate));
     tassert("ModifySDCount() ", sectionDesc.UpdateSDTblCount(16));
     tassert("ModifySDDtbl() " , sectionDesc.UpdateSDDynOffset(0x1337CAFFEEDDDDDD));
     tassert("ModifySDTreg() ", sectionDesc.UpdateSDRegOffset(0xEEEEEEEEEEEEEEEE));
     tassert("ReadSD() ", sectionDesc.Read());
#if defined(_LIBTRIDENT_DEBUG)
     print_sd(sectionDesc.GetObject());
#endif
    
    */

    /*
    {
    PkgIO::BinarySerializer bSer;
   //int oox = 0xbeefc;
   //unsigned char oox[] = {0xff, 0xaa};
    std::vector<uchar> oox = {0xff, 0xaa,0xfc,0xff, 0xaa,0xfc,0xff, 0xaa};
 
    bSer.EmptyData();
    bSer.AddType(oox);
   
   // tassert("BSWData() ", bSer.WriteData(true, 0, std::ios::end));
    auto haveCtx = bSer.GetFormattedData(true);
    if (haveCtx.has_value()) {
        const auto& vec = haveCtx.value();
      
        bSer.WriteDataToTStream(ltTrPkg.GetTstream(), vec, 0, std::ios::end);
       // tassert("BSWData() ",  == Err::Code::SUCCESS);
        
    }
    else {
        dbgprintf("BSWfail\n");
    }
     //std::cout << PkgIO::BinarySerializer::GetByteAlignment(sizeof(ccc)) << std::endl;
    //todo fr add those asserts
    //std::cout << "WritePadding()" << ltTrPkg.fstrInfo->WritePadding(32, 0xCCCC) << std::endl;
    }
   
  
  //write 
 
  
  //const auto& currentSeek = ltTrPkg.GetTstream().GetSeekPos();
  ltTrPkg.GetTstream().SetSeekPos(0, std::ios::end);
  {
    NTC_INFO_TEST ntc;
    ntc.x = 0xfde2;
    ntc.y = 0xfad13333;
    ntc.z = 0x2edf;
   // ntc.c = {UINT32_MAX, UINT32_MAX};
   // ntc.c = 'A';
    //later called using template 
    const auto& haveNtc = ntc.Serialize();
    if (haveNtc.has_value()) {
        const auto& val = haveNtc.value();
        PkgIO::BinarySerializer::WriteDataToTStream(ltTrPkg.GetTstream(), val, 0, std::ios::end);
    }
  }
  //read
  const auto& currentSeek = ltTrPkg.GetTstream().GetSeekPos();
  {
    NTC_INFO_TEST ntcRead;
    ntcRead.x = 0;
    ntcRead.y = 0;
    ntcRead.z = 0;
    const auto& ntcReadSize = ntcRead.Sizeof();
    std::cout << "ntc size: " << ntcReadSize << "\n";
    const auto readData =  PkgIO::BinarySerializer::ReadDataFromTStream(ltTrPkg.GetTstream(), currentSeek, ntcReadSize);
    
    if(!ntcRead.Deserialize(readData)) {
        return 1;
    }
    printf("ntc:%X/%X/%X\n", ntcRead.x, ntcRead.y, ntcRead.z);
  }
  

  
  //ltTrPkg.fstrInfo->WritePadding(32, 0xCCCC);
  
  // ltTrPkg.fstrInfo->WritePadding(32, 0xCCCC);
  
  //ClosePkg() not needed because of RAII
  //  ltTrPkg.ClosePkg();
  */
    auto portStat = ltTrPkg.getTstream().getFstreamObject().pStat;
    print_stat(portStat);
    std::cout << "Exit(0)\n";
    return 0;
    
}
