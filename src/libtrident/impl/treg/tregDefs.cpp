#include "tregDefs.h"
#include "dtbl/section/dynamicTable.h"
using namespace Trd;
using namespace Trd::Impl;
std::optional<std::vector<std::byte>> TregHeader::serialize() const {
    if (_reserved1 != 0 || magic != Consts::Treg::TREG_MAGIC)
        return std::nullopt;

    Trd::Impl::BinarySerializer bs;
    bs.addTrivial(magic, flags, keyEntriesCount, valEntriesCount, attrEntriesCount, poolSize,
                  checksum, _reserved1);
    return bs.getFormattedData();
}

bool TregHeader::deserialize(const std::vector<std::byte>& dataIn) {
    Impl::BinarySerializer bs(dataIn);

    bs.readTrivial(magic, flags, keyEntriesCount, valEntriesCount, attrEntriesCount, poolSize,
                   checksum, _reserved1);

    if (bs.getReadOffset() != this->size() || _reserved1 != 0) {
        return false;
    }
    return true;
}

std::optional<Impl::Crc32Gen> TregHeader::checksumCRC32() const {
    return std::nullopt;
}