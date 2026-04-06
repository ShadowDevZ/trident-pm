#include "sdescdefs.h"

using namespace Trd;
using namespace Impl;
std::optional<std::vector<u8>> TRD_SECTION_DESCRIPTOR::serialize() const {
    if (_reserved1 != 0 || _reserved2 != 0 || !u8b_valid(sdReady) || idByte != Consts::SD::TRD_SD_IDBYTE) {

        return std::nullopt;
    }
    Trd::Impl::BinarySerializer bs;
    bs.addTrivial(crc, sectionStatusCode, sdReady, tblCount, tblDynamicOffset, tblRegistryOffset, _reserved1, _reserved2, idByte);

    return bs.getFormattedData();
}
bool TRD_SECTION_DESCRIPTOR::deserialize(const std::vector<u8>& dataIn) {
    Impl::BinarySerializer bs(dataIn);

    bs.readTrivial(crc, sectionStatusCode, sdReady, tblCount, tblDynamicOffset, tblRegistryOffset, _reserved1, _reserved2, idByte);
    dbgprintf("xsize:%ld:\n", bs.getReadOffset());

    if (bs.getReadOffset() != this->size() || _reserved1 != 0 || _reserved2 != 0 || !u8b_valid(sdReady) || idByte != Consts::SD::TRD_SD_IDBYTE)
        return false;

    return true;
}
std::optional<u32> TRD_SECTION_DESCRIPTOR::checksumCRC32() const {
    if (_reserved1 != 0 || _reserved2 != 0 || !u8b_valid(sdReady) || idByte != Consts::SD::TRD_SD_IDBYTE) {
        return std::nullopt;
    }
    Impl::Crc32Gen crc;
    crc.addData(sectionStatusCode, tblCount, tblDynamicOffset, tblRegistryOffset, _reserved1, _reserved1, _reserved2, idByte);
    return crc.getCrc32();
}