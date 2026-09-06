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
void TregHeader::dbgInfoPrint() const {
    dbgprintf("\x1B[33m  [TregHeader]\n"
              "\tmagic: 0x%X\n\tflags: %u\n\tkeyEntriesCount: %u\n"
              "\tvalEntriesCount: %u\n\tattrEntriesCount: %u\n"
              "\tpoolSize: %lu\n\tchecksum: 0x%X\n\t_reserved1: %u\n  [TregHeader]\n\x1B[0m",
              magic, static_cast<u8>(flags), keyEntriesCount, valEntriesCount, attrEntriesCount,
              poolSize, checksum, _reserved1);
}

void TregKeyRecord::dbgInfoPrint() const {
    dbgprintf(
        "\x1B[33m  [TregKR]\n"
        "\tidentifier: 0x%X\n\tkeyNameOffset: %u\n\tkeyNameLength: %u\n"
        "\tfirstChildKeyIndex: %u\n\tchildKeysCount: %u\n"
        "\tfirstValIndex: %u\n\tvalueCount: 0x%X\n\trecordChecksum: 0x%X\n  [TregKR]\n\x1B[0m",
        identifier, keyNameOffset, keyNameLength, firstChildKeyIndex, childKeysCount, firstValIndex,
        valueCount, recordChecksum);
}
void TregValueRecord::dbgInfoPrint() const {
    dbgprintf("\x1B[33m  [TregVR]\n"
              "\tidentifier: 0x%X\n\tvalNameOffset: %u\n\tvalNameLength: %u\n"
              "\tattrFirstIndex: %u\n\tattrCount: %u\n\trecordChecksum: 0x%X\n  [TregVR]\n\x1B[0m",
              identifier, valNameOffset, valNameLength, attrFirstIndex, attrCount, recordChecksum);
}

void TregAttrRecord::dbgInfoPrint() const {
    dbgprintf("\x1B[33m  [TregAR]\n"
              "\tidentifier: 0x%X\n\tattrNamePoolOffset: %u\n\tattrNameLength: %u\n"
              "\tdatatype: %u\n\trecordChecksum: 0x%x\n",
              identifier, attrNamePoolOffset, attrNameLength, static_cast<u8>(datatype),
              recordChecksum);
    if (std::holds_alternative<TrivialData>(payload)) {
        TrivialData td = std::get<TrivialData>(payload);
        dbgprintf("\tpayload:Trivial\n\t[ ");
        for (const auto& x : td) {
            dbgprintf("%02X ", static_cast<u8>(x));
        }
        dbgprintf("]\n");
    } else if (std::holds_alternative<PayloadPoolData>(payload)) {
        PayloadPoolData ppd = std::get<PayloadPoolData>(payload);
        dbgprintf("\tpayload:PoolData\n\tppdOffset: %u\n\tppdSize: %u\n", ppd.payloadPoolOffset,
                  ppd.payloadSize);
    } else {
        dbgprintf("payload:INVALID_DATA\n");
    }
    dbgprintf("  [TregAR]\n\x1B[0m");
}
//std::optional<Impl::Crc32Gen> TregHeader::checksumCRC32() const {
//    return std::nullopt;
//}