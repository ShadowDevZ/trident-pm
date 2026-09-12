#include "tregFile.h"
#include "dtbl/section/dynamicTable.h"
using namespace Trd;
using namespace Trd::Impl;
std::optional<std::vector<std::byte>> TregHeader::serialize() const {
    if (_reserved1 != 0 || magic != Consts::Treg::TREG_MAGIC)
        return std::nullopt;

    Trd::Impl::BinarySerializer bs;
    bs.addTrivial(magic, flags, subKeysCount, subEntriesCount, subValsCount, poolSize, checksum,
                  _reserved1);
    return bs.getFormattedData();
}

bool TregHeader::deserialize(const std::vector<std::byte>& dataIn) {
    Impl::BinarySerializer bs(dataIn);

    bs.readTrivial(magic, flags, subKeysCount, subEntriesCount, subValsCount, poolSize, checksum,
                   _reserved1);

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
                  firstEntryIndex, entryCount, recordChecksum);
    return bs.getFormattedData();
}

bool TregKeyRecord::deserialize(const std::vector<std::byte>& dataIn) {
    Impl::BinarySerializer bs(dataIn);

    bs.readTrivial(identifier, keyNameOffset, keyNameLength, firstChildKeyIndex, childKeysCount,
                   firstEntryIndex, entryCount, recordChecksum);

    if (bs.getReadOffset() != this->size() || identifier != Consts::Treg::KEY_REC_ID) {
        return false;
    }
    return true;
}
std::optional<std::vector<std::byte>> TregEntryRecord::serialize() const {
    if (identifier != Consts::Treg::ENTRY_REC_ID) // todo checksum
        return std::nullopt;

    Trd::Impl::BinarySerializer bs(std::endian::native, false);
    bs.addTrivial(identifier, entryNameOffset, entryNameLength, valsFirstIndex, valsCount,
                  recordChecksum);
    return bs.getFormattedData();
}

bool TregEntryRecord::deserialize(const std::vector<std::byte>& dataIn) {
    Trd::Impl::BinarySerializer bs(std::endian::native, false);

    bs.readTrivial(identifier, entryNameOffset, entryNameLength, valsFirstIndex, valsCount,
                   recordChecksum);

    if (bs.getReadOffset() != this->size() || identifier != Consts::Treg::ENTRY_REC_ID) {
        return false;
    }
    return true;
}

std::optional<std::vector<std::byte>> TregValueRecord::serialize() const {
    if (identifier != Consts::Treg::VALUE_REC_ID) // todo checksum
        return std::nullopt;

    Trd::Impl::BinarySerializer bs(std::endian::native, false);

    bs.addTrivial(identifier, valNamePoolOffset, valNameLength, datatype, recordChecksum);
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

bool TregValueRecord::deserialize(const std::vector<std::byte>& dataIn) {
    Impl::BinarySerializer bs(dataIn);

    bs.readTrivial(identifier, valNamePoolOffset, valNameLength, datatype, recordChecksum);

    if (bs.getReadOffset() != this->size() || identifier != Consts::Treg::VALUE_REC_ID) {
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
              magic, static_cast<u8>(flags), subKeysCount, subEntriesCount, subValsCount, poolSize,
              checksum, _reserved1);
}

void TregKeyRecord::dbgInfoPrint() const {
    dbgprintf(
        "\x1B[33m  [TregKR]\n"
        "\tidentifier: 0x%X\n\tkeyNameOffset: %u\n\tkeyNameLength: %u\n"
        "\tfirstChildKeyIndex: %u\n\tchildKeysCount: %u\n"
        "\tfirstValIndex: %u\n\tentryCount: 0x%X\n\trecordChecksum: 0x%X\n  [TregKR]\n\x1B[0m",
        identifier, keyNameOffset, keyNameLength, firstChildKeyIndex, childKeysCount,
        firstEntryIndex, entryCount, recordChecksum);
}
void TregEntryRecord::dbgInfoPrint() const {
    dbgprintf("\x1B[33m  [TregVR]\n"
              "\tidentifier: 0x%X\n\tvalNameOffset: %u\n\tentryNameLength: %u\n"
              "\tattrFirstIndex: %u\n\tattrCount: %u\n\trecordChecksum: 0x%X\n  [TregVR]\n\x1B[0m",
              identifier, entryNameOffset, entryNameLength, valsFirstIndex, valsCount,
              recordChecksum);
}

void TregValueRecord::dbgInfoPrint() const {
    dbgprintf("\x1B[33m  [TregAR]\n"
              "\tidentifier: 0x%X\n\tattrNamePoolOffset: %u\n\tattrNameLength: %u\n"
              "\tdatatype: %u\n\trecordChecksum: 0x%x\n",
              identifier, valNamePoolOffset, valNameLength, static_cast<u8>(datatype),
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

void Entry::IManageValue(const Value& a, bool overwrite) {
    bool foundDuplicit = false;
    for (const auto& x : values) {
        if (x.getName() == a.getName()) {
            foundDuplicit = true;
            break;
        }
    }
    if (foundDuplicit && !overwrite) {
        throw std::runtime_error("Tried to add existing entry");
    } else if (foundDuplicit && overwrite) {
        Value* oldAttr = findValue(a.getName());
        [[unlikely]]
        if (oldAttr == nullptr) //just in case
            throw std::runtime_error("Attribute does not exist");
        *oldAttr = a; //replace the old one

    } else {
        values.push_back(std::move(a));
    }
}

bool Entry::deleteValue(std::string_view name) {
    auto noDeleted =
        std::erase_if(values, [name](const Value& attr) { return attr.getName() == name; });
    if (noDeleted == 0)
        return false;
    return true;
}

Key* Key::findChild(std::string_view name) {
    for (auto& x : children) {
        if (x.getName() == name)
            return &x;
    }
    return nullptr;
}
Entry* Key::findEntry(std::string_view name) {
    for (auto& x : entries) {
        if (x.getName() == name)
            return &x;
    }
    return nullptr;
}

Value const* Entry::findValue(std::string_view name) const {
    for (const auto& x : values) {
        if (x.getName() == name)
            return &x;
    }
    return nullptr;
}

Value* Entry::findValue(std::string_view name) {
    for (auto& x : values) {
        if (x.getName() == name)
            return &x;
    }
    return nullptr;
}

void printValues(const Entry& entry, const std::string& prefix) {
    auto entries = entry.getValues();
    for (size_t i = 0; i < entries.size(); ++i) {
        bool isLast = false;
        if (i + 1 == entries.size())
            isLast = true;
        const auto& e = entries[i];
        auto entryName = e.getName();
        auto type = Value::DatatypeAsString(e.getValueDataType()).value_or("???");
        dbgprintf("%s%s\x1B[33m[%.*s] (type.%s, vid.%lu)\n\x1B[0m", prefix.c_str(),
                  isLast ? "└──" : "├──", static_cast<int>(entryName.size()), entryName.data(),
                  type, e.getPayload().index());
    }
}
std::optional<const char*> Value::DatatypeAsString(TregValueDatatype vd) {
    /*
      i8 = 0,
        i16 = 1,
        i32 = 2,
        i64 = 3,
        u8 = 4,
        u16 = 5,
        u32 = 6,
        u64 = 7,
        u8_bool = 10,
        cstr = 11,
        binDataLE = 12,
        //   binDataBE = 13 //not implemented currently
        //symLink = 14 // link to another key,value*/
    using v = TregValueDatatype;
    static constexpr std::pair<TregValueDatatype, const char*> dataString[] = {
        {v::i8, "i8"},           {v::i16, "i16"},   {v::i32, "i32"},    {v::i64, "i64"},
        {v::u8, "u8"},           {v::u16, "u16"},   {v::u32, "u32"},    {v::u64, "u64"},
        {v::u8_bool, "u8_bool"}, {v::cstr, "cstr"}, {v::binLE, "binLE"}};

    for (const auto& [flag, text] : dataString) {
        if (flag == vd)
            return text;
    }
    return std::nullopt;
}

void Key::printTree(const Key& k, const std::string& prefix, bool isRoot) {
    if (isRoot) {
        auto name = k.getName();
        dbgprintf("\x1B[36m%.*s\n\x1B[0m", static_cast<int>(name.size()), name.data());
    }
    struct DirPrintList {
        std::string_view name;
        bool isKey;
        size_t idx;
    };
    std::vector<DirPrintList> list;
    auto children = k.getChildren();
    auto entries = k.getEntries();
    for (size_t i = 0; i < children.size(); ++i) {
        list.push_back({children[i].getName(), true, i});
    }
    for (size_t i = 0; i < entries.size(); ++i) {
        list.push_back({entries[i].getName(), false, i});
    }
    std::ranges::sort(list, {}, [](const DirPrintList& p) { return p.name; });
    for (size_t i = 0; i < list.size(); ++i) {
        bool isLast = false;
        if (i + 1 == list.size())
            isLast = true;

        std::string childPrefix = prefix + (isLast ? "    " : "│   ");
        if (list[i].isKey) {
            dbgprintf("%s%s\x1B[31m[%.*s]\n\x1B[0m", prefix.c_str(), isLast ? "└──" : "├──",
                      static_cast<int>(list[i].name.size()), list[i].name.data());
            printTree(children[list[i].idx], childPrefix, false);
        } else {
            dbgprintf("%s%s\x1B[32m[%.*s]\n\x1B[0m", prefix.c_str(), isLast ? "└──" : "├──",
                      static_cast<int>(list[i].name.size()), list[i].name.data());
            printValues(entries[list[i].idx], childPrefix);
        }
    }
}
//std::optional<Impl::Crc32Gen> TregHeader::checksumCRC32() const {
//    return std::nullopt;
//}