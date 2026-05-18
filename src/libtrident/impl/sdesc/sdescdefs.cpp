#include "sdescdefs.h"
#include "sdescHelper.h"
using namespace Trd;
using namespace Impl;

std::optional<std::vector<u8>> TRD_SECTION_DESCRIPTOR::serialize() const {
    if (!IResvFieldCheck(_reserved2) || _reserved1 != 0 || idByte != Consts::SD::TRD_SD_IDBYTE) {

        return std::nullopt;
    }
    Trd::Impl::BinarySerializer bs;
    bs.addTrivial(crc, sdReady, tblDynamic.offset, tblDynamic.size, tblDynamic.available,
                  tblRegistry.offset, tblRegistry.size, tblRegistry.available, _reserved2,
                  _reserved1, idByte);

    return bs.getFormattedData();
}
bool TRD_SECTION_DESCRIPTOR::deserialize(const std::vector<u8>& dataIn) {
    Impl::BinarySerializer bs(dataIn);

    bs.readTrivial(crc, sdReady, tblDynamic.offset, tblDynamic.size, tblDynamic.available,
                   tblRegistry.offset, tblRegistry.size, tblRegistry.available, _reserved2,
                   _reserved1, idByte);

    if (bs.getReadOffset() != this->size() || !IResvFieldCheck(_reserved2) || _reserved1 != 0 ||
        idByte != Consts::SD::TRD_SD_IDBYTE)
        return false;

    return true;
}
std::optional<u32> TRD_SECTION_DESCRIPTOR::checksumCRC32() const {
    if (_reserved1 != 0 || !IResvFieldCheck(_reserved2) || idByte != Consts::SD::TRD_SD_IDBYTE) {
        return std::nullopt;
    }
    Impl::Crc32Gen crc;

    crc.addData(tblDynamic.offset, tblDynamic.size, tblDynamic.available, tblRegistry.offset,
                tblRegistry.size, tblRegistry.available, _reserved2, _reserved1, idByte);
    return crc.getCrc32();
}