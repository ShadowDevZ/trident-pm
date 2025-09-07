
#include <string.h>
#include <iostream>
#include "libtrident.h"
#include "tuid.h"

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


void print_header(LibTrident::Header::TRD_HEADER& hdr) {
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
bool IsLittleEndian () {

    int i=1;

    return (int)*((unsigned char *)&i)==1;

}

int main(void) {
    PrintBuildTarget();
   
    LibTrident::TrPkg lt;
    lt.OpenPackage("/home/shadow/Projects/trident-pm/test.tpx", IOFLAGS::ACCESS_RW | IOFLAGS::CREATE_NEW);
    std::cout << "Last Error: " << lt.e << std::endl;
    
    LibTrident::Header::TRD_HEADER h;
    LibTrident::Header::TRD_HEADER hdrReadBack;
    LibTrident::Header::TRDPkgHeader x(lt.fstrInfo);
#ifdef _LIBTRIDENT_DEBUG
    TRDSdToken tokenId(lt.fstrInfo);
#endif

    

   // std::cout << lt.fstrInfo->GetFileStreamInfo()->name;
    
    
   std::cout << std::boolalpha;
   std::cout << "CreateHeader()" <<x.CreateNewHeader(h, BF_PLATF_LINUX, ARCHT_AM64, COMMPRALG_NONE) << std::endl;
   print_header(h);
   std::cout << "WriteHeader() " << x.WriteHeader(h) << std::endl;
   std::cout << "HeaderRBValid() " << x.IsWrittenHeaderValid() << std::endl;
    std::cout << "HeaderPresent() " << x.ReadHeader(hdrReadBack) << std::endl;
#ifdef _LIBTRIDENT_DEBUG
    std::cout << "WriteSDToken(beg)" << tokenId.WriteDescriptorTUID(true) << std::endl;
    //temporary replacement for BlankSD();
    lt.fstrInfo->GetFileStreamInfo()->hFile->seekp(70);
   // char data[200] = {0};
   // lt.fstrInfo->GetFileStreamInfo()->hFile->write(data, 200);
     std::cout << "WriteSDToken(end)" << tokenId.WriteDescriptorTUID(false) << std::endl;
#endif
    std::cout << std::noboolalpha;
    std::cout << x.e.GetErrorAsString() << std::endl;
   // std::cout << TRDPkgHeader::GetHeaderByteSize() + 1 + TUID::TUID_MAX_LENGTH + 1;
    
    lt.ClosePkg();
    
    // std::cout << TUID::IsValidTUID("7a153cca-f082-4837-9f8b-10905d006261") << std::endl;


    return 0;
}
