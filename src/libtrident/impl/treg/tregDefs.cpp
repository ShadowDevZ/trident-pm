#include "tregDefs.h"
#include "dtbl/section/dynamicTable.h"
using namespace Trd;
using namespace Trd::Impl;
std::optional<std::vector<std::byte>> TregHeader::serialize() const {
    if (_reserved != 0 || _reserved1 != 0 || magic != Consts::Treg::TREG_MAGIC ||
        regRootOffset < DtblDirectory::badOffset())
        return std::nullopt;

    Trd::Impl::BinarySerializer bs;
    bs.addTrivial(magic, flags, regRootOffset, regSizeTotal, _reserved, _reserved1);
    return bs.getFormattedData();
}

bool TregHeader::deserialize(const std::vector<std::byte>& dataIn) {
    return false;
}

std::optional<u32> TregHeader::checksumCRC32() const {
    return std::nullopt;
}