#include "tregFile.h"
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

std::optional<std::vector<std::byte>> TregKeyRecord::serialize() const {
    if (identifier != Consts::Treg::KEY_REC_ID) // todo checksum
        return std::nullopt;

    Trd::Impl::BinarySerializer bs(std::endian::native, false);
    bs.addTrivial(identifier, keyNameOffset, keyNameLength, firstChildKeyIndex, childKeysCount,
                  firstValIndex, valueCount, recordChecksum);
    return bs.getFormattedData();
}

bool TregKeyRecord::deserialize(const std::vector<std::byte>& dataIn) {
    Impl::BinarySerializer bs(dataIn);

    bs.readTrivial(identifier, keyNameOffset, keyNameLength, firstChildKeyIndex, childKeysCount,
                   firstValIndex, valueCount, recordChecksum);

    if (bs.getReadOffset() != this->size() || identifier != Consts::Treg::KEY_REC_ID) {
        return false;
    }
    return true;
}
std::optional<std::vector<std::byte>> TregValueRecord::serialize() const {
    if (identifier != Consts::Treg::VAL_REC_ID) // todo checksum
        return std::nullopt;

    Trd::Impl::BinarySerializer bs(std::endian::native, false);
    bs.addTrivial(identifier, valNameOffset, valNameLength, attrFirstIndex, attrCount,
                  recordChecksum);
    return bs.getFormattedData();
}

bool TregValueRecord::deserialize(const std::vector<std::byte>& dataIn) {
    Trd::Impl::BinarySerializer bs(std::endian::native, false);

    bs.readTrivial(identifier, valNameOffset, valNameLength, attrFirstIndex, attrCount,
                   recordChecksum);

    if (bs.getReadOffset() != this->size() || identifier != Consts::Treg::VAL_REC_ID) {
        return false;
    }
    return true;
}

std::optional<std::vector<std::byte>> TregAttrRecord::serialize() const {
    if (identifier != Consts::Treg::ATTR_REC_ID) // todo checksum
        return std::nullopt;

    Trd::Impl::BinarySerializer bs(std::endian::native, false);

    bs.addTrivial(identifier, attrNamePoolOffset, attrNameLength, datatype, recordChecksum);
    if (AttrIsTrivial(datatype) && std::holds_alternative<TrivialData>(payload)) {
        // bs.addContainer(std::get<TrivialData>(payload));
        auto pData = std::get<TrivialData>(payload);
        bs.addContainer(std::span<const std::byte>(pData));
    } else if (!AttrIsTrivial(datatype) && std::holds_alternative<PayloadPoolData>(payload)) {
        auto pData = std::get<PayloadPoolData>(payload);
        bs.addTrivial(pData.payloadPoolOffset, pData.payloadSize);
    } else {
        return std::nullopt;
    };
    return bs.getFormattedData();
}

bool TregAttrRecord::deserialize(const std::vector<std::byte>& dataIn) {
    Impl::BinarySerializer bs(dataIn);

    bs.readTrivial(identifier, attrNamePoolOffset, attrNameLength, datatype, recordChecksum);

    if (bs.getReadOffset() != this->size() || identifier != Consts::Treg::ATTR_REC_ID) {
        return false;
    }

    if (AttrIsTrivial(datatype)) {
        TrivialData trivialData;
        bs.readContainer(std::span<std::byte>(trivialData));
        payload.emplace<TrivialData>(trivialData);
    } else {
        PayloadPoolData pData;
        bs.readTrivial(pData.payloadPoolOffset, pData.payloadSize);
        payload.emplace<PayloadPoolData>(pData);
    }
    return true;
}
//std::optional<Impl::Crc32Gen> TregHeader::checksumCRC32() const {
//    return std::nullopt;
//}