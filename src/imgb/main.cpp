#include <cstring>
#include <iostream>
#include "libtrident.h"
#include "sdesc.h"
#include <array>
#include "serdatacommon.h"
#include "trheader.h"
#include <cstdlib>
#include "systemspecific/common/include/filemgmnt.h"

#include "mainDbgHelp.h"
//TODO THIS FILE SHOULD CONTAIN STATIC_ASSERTIONS
using namespace Trd;

#include "err/callback.h"

/*
void cb_data(const Trd::Err::TrdError& e) {
    std::cout << "callback reg called!\n";
    std::cout << "got error " << e << "\n";
}
*/
int main(void) {
    // std::cout << SystemSpecific::createTemporaryFile().value() << std::endl;

    PrintBuildTarget();

    Trd::TrPkg ltTrPkg(
        "./test.tpx",
        {TrdOpenIO::CreateNew, TrdAccessIO::AllAccess, TrdXattrIO::None, _TrdInternalIO::None});
    //  Trd::TrPkg ltTrPkg("./test.tpx", IOFLAGS::TrdOpenIO::CreateNew, IOFLAGS::TrdAccessIO::ReadAll);
    //ltTrPkg.OpenPackage("./test.tpx", IOFLAGS::ACCESS_RW | IOFLAGS::CREATE_NEW);

    //u32 reg = Err::TrdErrorCallback::instance().registerCallback(cb_data);

    auto trPkgHdr = ltTrPkg.header();

#ifdef _LIBTRIDENT_DEBUG

#endif

    TASSERT("CreateHeader()",
            trPkgHdr.create(BuildFlags::PlatformLinux, ArchType::Amd64, GlobalCompression::None));
#ifdef _LIBTRIDENT_DEBUG
    print_header(trPkgHdr.getHeader());
#endif
    // NOLINTNEXTLINE
    TASSERT("WriteHeader()", trPkgHdr.write());
    // std::cout << "HeaderRBValid() " << trPkgHdr.IsWrittenHeaderValid() << std::endl;

    TASSERT("ReadHeader()", trPkgHdr.read());

    TRD_HDRFIELD_UPDATE updateField;
    updateField.architecture = ArchType::Aarch64;
    updateField.buildFlags = BuildFlags::PlatformNT;
    updateField.compression = GlobalCompression::GZip;
    updateField.fmtVersion = TrFileHeader::formatHeaderVersion(3, 1, 2).value();

    TASSERT("UpdateHeader()", trPkgHdr.updateHeader(updateField));

    TASSERT("ValidateHeader()", trPkgHdr.isValid());

    //#if defined(_LIBTRIDENT_DEBUG)
    //   print_header(trPkgHdr.getHeader());
    //#endif

    TASSERT("ModifyLen()", trPkgHdr.updateFileLenProp(0xbeefccaa));
    TASSERT("ValidateHeader()", trPkgHdr.isValid());
#ifdef _LIBTRIDENT_DEBUG
    auto vxa = trPkgHdr.getHeader();
    print_header(vxa);
#endif
    auto trpkgSD = ltTrPkg.sd();
    TASSERT("CreateSD()", trpkgSD.createWriteBlank());
    // print_sd(trpkgSD.getSD());
    Impl::SD_TBLENTRY dynamic{true};
    dynamic.offset = 0x1337CAFFEEDDDDDD;
    dynamic.size = 8;
    dynamic.available = u8bool::type::True;
    Impl::SD_TBLENTRY regt{true};
    regt.offset = 0xEEEEEEEEEEEEEEEE;
    regt.size = 8;

    Impl::TRD_SD_UPDATEFIELD sdUpd;
    sdUpd.tblDynamic = dynamic;
    sdUpd.tblRegistry = regt;

    trpkgSD.changeReadyStatus(true);
    TASSERT("UpdateSD()", trpkgSD.updateSD(sdUpd));

    auto rbSd = trpkgSD.getSD();
    TASSERT("ReadbackSD()", trpkgSD.read());
    if (rbSd != ltTrPkg.sd().getSD()) {
        dbgprintf("eread!fail\n");
        return 1;
    }

#ifdef _LIBTRIDENT_DEBUG
    print_sd(rbSd);
#endif

    std::cout << "Exit(0)\n";
    return 0;
}
