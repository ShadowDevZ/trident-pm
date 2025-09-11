
#include <string.h>
#include <iostream>
#include "libtrident.h"
#include "suid.h"

#ifdef _LIBTRIDENT_DEBUG
#include "sdescid.h"
#endif
using namespace LibTrident;
using namespace LibTrident::Header;

void PrintBuildTarget() {
#ifdef _LIBTRIDENT_DEBUG
std::cout << "Target: Debug\n\n";    
#else
std::cout << "Target: Release\n"; 
#endif
}

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
   // return 0;
    //LibTrident::Header::TRD_HEADER h;
    //LibTrident::Header::TRD_HEADER hdrReadHeaderBack;
    LibTrident::Header::TRDPkgHeader x(lt.fstrInfo);
#ifdef _LIBTRIDENT_DEBUG
    TRDSdToken tokenId(lt.fstrInfo);
#endif

    

   // std::cout << lt.fstrInfo->GetFstreamObject()->name;
    
    
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
    std::cout << "ModifyLen() " << x.SetFileLen(0xbeefccaa, true) << std::endl;
    std::cout << "ModifyIOCTRL() " << x.SetIoctrl(IOCTRL_DSEC_RLOCK, true) << std::endl;
    std::cout << "ValidateHeader() " << x.IsValid() << std::endl;
    print_header(x.GetInternal());   

#ifdef _LIBTRIDENT_DEBUG
    std::cout << "WriteHeaderSDToken(beg)" << tokenId.WriteHeaderDescriptorSUID(true) << std::endl;
    //temporary replacement for BlankSD();
    if (lt.fstrInfo->CheckFileStreamInfo()) {
        lt.fstrInfo->GetFstreamObject().hFile->seekp(70);

    }
   // char data[200] = {0};
   // lt.fstrInfo->GetFstreamObject()->hFile->WriteHeader(data, 200);
     std::cout << "WriteHeaderSDToken(end)" << tokenId.WriteHeaderDescriptorSUID(false) << std::endl;
#endif
   
    std::cout << std::noboolalpha;
    std::cout << x.e.GetErrorAsString() << std::endl;
   // std::cout << TRDPkgHeader::GetHeaderByteSize() + 1 + TUID::TUID_MAX_LENGTH + 1;

    lt.ClosePkg();
    std::cout << "Exit(0)\n";
    return 0;
    
}
