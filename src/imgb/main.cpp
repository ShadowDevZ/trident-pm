
#include <string.h>
#include <iostream>
#include "libtrident.h"
#include "suid.h"
#include "sdesc.h"

//TODO THIS FILE SHOULD CONTAIN STATIC_ASSERTIONS
using namespace LibTrident;
using namespace LibTrident::Header;
using namespace LibTrident::SectionDescriptor;

void PrintBuildTarget() {
#ifdef _LIBTRIDENT_DEBUG
std::cout << "Target: Debug\n\n";    
#else
std::cout << "Target: Release\n"; 
#endif
}
#include <vector>


#ifdef _LIBTRIDENT_DEBUG
void print_header(const LibTrident::Header::TRD_HEADER& hdr) {
    dbgprintf("[HEADER_START]\n");
    dbgprintf("\tMagic: ");
    for (auto const& it: hdr.magic) {
        dbgprintf("%X ", it);
    }
    dbgprintf("\n");
    dbgprintf("\tExtened Signature: 0x%X\n",hdr.exSignature);
    dbgprintf("\tVersion Format %s\n", 
    TRDPkgHeader::HeaderVersionFormatToString(hdr.fmtVersion).c_str());
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
#endif

int main(void) {
    PrintBuildTarget();
   
    LibTrident::TrPkg lt("./test.tpx", IOFLAGS::ACCESS_RW | IOFLAGS::CREATE_NEW);
    //lt.OpenPackage("./test.tpx", IOFLAGS::ACCESS_RW | IOFLAGS::CREATE_NEW);



    LibTrident::Header::TRDPkgHeader x(lt.fstrInfo);
#ifdef _LIBTRIDENT_DEBUG
  
#endif

    

   
   
   
   
   //std::cout << "CreateHeader()" <<x.Create(BF_PLATF_LINUX, ARCHT_AM64, COMMPRALG_NONE) << std::endl;
   tassert("CreateHeader()",x.Create(BF_PLATF_LINUX, ARCHT_AM64, COMMPRALG_NONE));
#ifdef _LIBTRIDENT_DEBUG
   print_header(x.GetObject());
#endif
  tassert("WriteHeaderHeader()" , x.Write())
  // std::cout << "HeaderRBValid() " << x.IsWrittenHeaderValid() << std::endl;
    tassert("ReadHeader() ", x.Read());
    
    TRD_HDRFIELD_UPDATE updateField;
    updateField.architecture = ARCHT_AARCH64;
    updateField.buildFlags = BF_PLATF_NT;
    updateField.compression = COMPRALG_GZIP;
    updateField.fmtVersion = TRDPkgHeader::FormatHeaderVersion(3,1,2);
    
    tassert("UpdateHeader() ", x.UpdateHeader(updateField));
 
    tassert("ValidateHeader() ", x.IsValid());
    print_header(x.GetObject());   
   
    tassert("ModifyLen() ", x.UpdateFileLenProp(0xbeefccaa));
    tassert("ModifyIOCTRL() ", x.UpdateIoctrlProp(IOCTRL_DESC_WLOCK));
    tassert("ValidateHeader() ", x.IsValid());
    print_header(x.GetObject());   

    TRDSecDesc sectionDesc(lt.fstrInfo);
    tassert("WriteBlankSD() ", sectionDesc.WriteBlankSD());
    TRD_SD_UPDATEFIELD sdUpdate;
    sdUpdate.tblCount = UINT32_MAX;
    sdUpdate.tblDynamicOffset = UINT64_MAX;
    sdUpdate.tblRegistryOffset = UINT64_MAX;
    tassert("UpdateSD() ", sectionDesc.UpdateSD(sdUpdate));
    tassert("ModifySDCount() ", sectionDesc.UpdateSDTblCount(16));
    tassert("ModifySDDtbl() " , sectionDesc.UpdateSDDynOffset(0x1337CAFFEE));
    tassert("ModifySDTreg() ", sectionDesc.UpdateSDRegOffset(0xDEADBEEF));
    tassert("ReadSD() ", sectionDesc.Read());
    print_sd(sectionDesc.GetObject());
    PkgIO::BinarySerializer bSer(lt.fstrInfo);
    int oox = 0xbeefc;
    
    bSer.AddTrivial(oox);
   
    bSer.WriteData(0, std::ios::end);
   
   //std::cout << PkgIO::BinarySerializer::GetByteAlignment(sizeof(ccc)) << std::endl;
    //todo fr add those asserts
   // std::cout << "WritePadding()" << lt.fstrInfo->WritePadding(32, 0xCCCC) << std::endl;
   
    
    std::cout << x.e << std::endl;
   
//ClosePkg() not needed because of RAII
  //  lt.ClosePkg();
    std::cout << "Exit(0)\n";
    return 0;
    
}
