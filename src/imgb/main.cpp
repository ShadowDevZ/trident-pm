
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
#endif

int main(void) {
    PrintBuildTarget();
   
    LibTrident::TrPkg lt;
    bool openStatus = lt.OpenPackage("./test.tpx", IOFLAGS::ACCESS_RW | IOFLAGS::CREATE_NEW);
    std::cout << "Last Error: " << lt.e << " OpenPkg(): " << std::boolalpha << openStatus << std::noboolalpha << std::endl;

//THE REASON WE ARE CHECKING ONLY IN RELEASE IS BECAUSE IF THE PACKAGE FAILED TO OPEN ANY SUBSEQUENT FUNCTION MUST FAIL AND NOT USE IO
#ifndef _LIBTRIDENT_DEBUG
    if (!openStatus) {
        std::cout << "Failed to open package\n";
        return 1;
    }
#endif
    LibTrident::Header::TRDPkgHeader x(lt.fstrInfo);
#ifdef _LIBTRIDENT_DEBUG
  
#endif

    

   
    
   std::cout << std::boolalpha;
   std::cout << "CreateHeader()" <<x.Create(BF_PLATF_LINUX, ARCHT_AM64, COMMPRALG_NONE) << std::endl;
#ifdef _LIBTRIDENT_DEBUG
   print_header(x.GetInternal());
#endif
   std::cout << "WriteHeaderHeader() " << x.WriteHeader() << std::endl;
  // std::cout << "HeaderRBValid() " << x.IsWrittenHeaderValid() << std::endl;
    std::cout << "ReadHeader() " << x.ReadHeader() << std::endl;
    std::cout << std::endl;
    TRD_HDRFIELD_UPDATE updateField;
    updateField.architecture = ARCHT_AARCH64;
    updateField.buildFlags = BF_PLATF_NT;
    updateField.compression = COMPRALG_GZIP;
    updateField.fmtVersion = TRDPkgHeader::FormatHeaderVersion(3,1,2);
    std::cout << "UpdateHeader() " << x.UpdateHeader(updateField) << std::endl;
 
    std::cout << "ValidateHeader() " << x.IsValid() << std::endl;
    print_header(x.GetInternal());   
    std::cout << std::endl;
    std::cout << "ModifyLen() " << x.UpdateFileLenProp(0xbeefccaa) << std::endl;
    std::cout << "ModifyIOCTRL() " << x.UpdateIoctrlProp(IOCTRL_DSEC_RLOCK) << std::endl;
    std::cout << "ValidateHeader() " << x.IsValid() << std::endl;
    print_header(x.GetInternal());   

    TRDSecDesc sectionDesc(lt.fstrInfo);
    std::cout << "WriteBlankSD() " << sectionDesc.WriteBlankSD() << std::endl;
    TRD_SD_UPDATEFIELD sdUpdate;
    sdUpdate.tblCount = 0x777;
    sdUpdate.tblDynamicOffset = 0x1337CAFFEE;
    sdUpdate.tblRegistryOffset = 0xDEADBEEF;
    std::cout << "UpdateSD() " << sectionDesc.UpdateSD(sdUpdate) << std::endl;
    std::cout << "ModifySDCount() " << sectionDesc.UpdateSDTblCount(UINT32_MAX) << std::endl;
    std::cout << "ModifySDDtbl() " << sectionDesc.UpdateSDDynOffset(UINT64_MAX) << std::endl;
    std::cout << "ModifySDTreg() " << sectionDesc.UpdateSDRegOffset(UINT64_MAX) << std::endl;
   
    std::cout << std::noboolalpha;
    std::cout << x.e.GetErrorAsString() << std::endl;
   
    lt.ClosePkg();
    std::cout << "Exit(0)\n";
    return 0;
    
}
