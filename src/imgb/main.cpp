#include <cstring>
#include <iostream>
#include "libtrident.h"
#include "sdesc.h"
#include <array>
#include "serdatacommon.h"
#include "trheader.h"
#include <cstdlib>
#include "systemspecific/common/include/filemgmnt.h"
#include "dynamicTable.h"
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
    //Header
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
    //SD
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

    //DTBL

    Trd::DtblDirectory dtblDir(ltTrPkg);
    //value_or to fix compiler warning, if it indeed returns 0 then writeRawRegion will fail
    auto dtblOffset = dtblDir.getOffset().value_or(0);

    constexpr auto dummySize = UINT8_MAX;
    TASSERT("DtblWriteSDEntry()", dtblDir.writeSDEntry(dtblOffset, dummySize, false));

    std::vector<u8> data(dummySize, 0xFF);

    {
        BenchDbgTimer t("DtblWriteChunk");
        // TASSERT("DtblwriteRawRegion()", dtblDir.writeRawRegion(data, dtblOffset));
        TASSERT("DtblwriteRawRegion()", dtblDir.writeRegionInChunks(data, dtblOffset));
    }
    /*
    auto checkRead = dtblDir.readRawRegion(dtblOffset, dummySize);
    if (!checkRead) {
        dbgprintf("dtbl readback fail\n");
        return 1;
    }
    const auto& readData = checkRead.value();
    dump_arr(std::span{readData});
    if (readData != data) {
        dbgprintf("\ndtbl read check failed\n");
        return 1;
    }
    */

    TASSERT("readRegionChunkSetup()", dtblDir.readRegionChunkSetup(dtblOffset, dummySize));
    bool next = false;
    int num = 1;
    do {
        const auto& zv = dtblDir.readNextRegionChunk();
        next = zv.has_value();
        if (!next)
            break;
        const auto& da = zv.value();

        dump_arr(std::span{da.data});
        dbgprintf("\nchunk_no%d\n", num++);
    } while (next);

    dbgprintf("\ndtbl read ok\n");

    //output
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
