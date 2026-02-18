
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
using namespace Trd;



void PrintBuildTarget() {
#if defined(_LIBTRIDENT_DEBUG)
std::cout << "Target: Debug\n\n";    
#else
std::cout << "Target: Release\n"; 
#endif
}
#include <vector>


#if defined(_LIBTRIDENT_DEBUG)

void print_header(const Trd::TRD_HEADER& hdr) {
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
    dbgprintf("\tChecksum 0x%X\n", hdr.dynHdrChksum);
    dbgprintf("\tFile length 0x%lXB\n", hdr.dynFileLen);
    dbgprintf("\tIoControl 0x%X\n", hdr.dynIoCtrl);
    dbgprintf("[HEADER_END]\n");
}
void print_sd(const Trd::Impl::TRD_SD& sd) {
    dbgprintf("\n[SD_START]\n");
    dbgprintf("\tCRC: 0x%X\n", sd.crc);
    dbgprintf("\tTblcount: %u\n", sd.tblCount);
    dbgprintf("\tDtbl offset: 0x%lX\n", sd.tblDynamicOffset);
    dbgprintf("\tTreg offset: 0x%lX\n", sd.tblRegistryOffset);
    dbgprintf("\tReserved: %lu\n", sd._reserved0);
    dbgprintf("[SD_END]\n\n");
}

void print_stat(const Impl::PortableStat& ps) {
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
struct NTC_INFO_TEST : Impl::SerializableData{
    uint16_t x = 0; 
    uint32_t y = 0;
    uint16_t z = 0;

    u64 size() const override {
        return Impl::BinarySerializer::elementSize(x,y,z);
    }
    //std::array<uint32_t,2> c{};

//the sum of sizeof of all elements must be properly aligned
    std::optional<std::vector<u8>> serialize() const override {
        Impl::BinarySerializer bs;
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
        Impl::BinarySerializer bs(dataIn);
        u64 xsize = 0;
        xsize += bs.readTrivialEx<u16>(xsize, x);
        xsize += bs.readTrivialEx<u32>(xsize, y);
        xsize += bs.readTrivialEx<u16>(xsize, z);
        
        
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
   
    Trd::TrPkg ltTrPkg("./test.tpx", {TrdOpenIO::CreateNew,
                                        TrdAccessIO::AllAccess,
                                        TrdXattrIO::None,
                                        _TrdInternalIO::None});
  //  Trd::TrPkg ltTrPkg("./test.tpx", IOFLAGS::TrdOpenIO::CreateNew, IOFLAGS::TrdAccessIO::ReadAll);
    //ltTrPkg.OpenPackage("./test.tpx", IOFLAGS::ACCESS_RW | IOFLAGS::CREATE_NEW);



    auto trPkgHdr = ltTrPkg.header();
    
#if defined(_LIBTRIDENT_DEBUG)
  
#endif

    

   
   
    
   
   //std::cout << "CreateHeader()" <<x.Create(BF_PLATF_LINUX, ARCHT_AM64, COMMPRALG_NONE) << std::endl;
    tassert("CreateHeader()",trPkgHdr.create(BuildFlags::PlatformLinux, ArchType::Amd64, GlobalCompression::None));
#if defined(_LIBTRIDENT_DEBUG)
   print_header(trPkgHdr.getHeader());
#endif
// NOLINTNEXTLINE
    tassert("WriteHeader()" , trPkgHdr.write());
  // std::cout << "HeaderRBValid() " << trPkgHdr.IsWrittenHeaderValid() << std::endl;

  tassert("ReadHeader()", trPkgHdr.read());
    
    TRD_HDRFIELD_UPDATE updateField;
    updateField.architecture = ArchType::Aarch64;
    updateField.buildFlags = BuildFlags::PlatformNT;
    updateField.compression = GlobalCompression::GZip;
    updateField.fmtVersion = TRDPkgHeader::formatHeaderVersion(3,1,2).value();
    
    tassert("UpdateHeader()", trPkgHdr.updateHeader(updateField));
 
    tassert("ValidateHeader()", trPkgHdr.isValid());
#if defined(_LIBTRIDENT_DEBUG)
    print_header(trPkgHdr.getHeader());   
#endif
    tassert("ModifyLen()", trPkgHdr.updateFileLenProp(0xbeefccaa));
    tassert("ModifyIOCTRL()", trPkgHdr.updateIoctrlProp(PackageIOCtrl::DynamicSectionWriteLock));
    tassert("ValidateHeader()", trPkgHdr.isValid());
#if defined(_LIBTRIDENT_DEBUG)
    auto vxa = trPkgHdr.getHeader();
    print_header(vxa);
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
 
    std::cout << "Exit(0)\n";
    return 0;
    
}
