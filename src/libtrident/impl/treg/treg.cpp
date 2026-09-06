#include "treg.h"
#include "datatypes.h"
#include "libtrident.h"
using namespace Trd;
struct TregHiveSerializer::KeyMapPool {
    const Trd::Key* key;
    u32 childStart{0};
    u32 childCount{0};
};
std::vector<TregHiveSerializer::KeyMapPool> TregHiveSerializer::iRecordKeys(const Key& root) {
    std::vector<KeyMapPool> plan;
    plan.push_back({&root, 0, 0});
    for (size_t i = 0; i < plan.size(); ++i) {
        std::vector<const Trd::Key*> sorted = iSortByName(plan[i].key->getChildren());
        plan[i].childStart = static_cast<u32>(plan.size());
        plan[i].childCount = static_cast<u32>(sorted.size());
        for (const Key* k : sorted) {
            plan.push_back({k, 0, 0});
        }
    }
    return plan;
}
std::pair<u32, u16> TregHiveSerializer::iPoolAppendName(PoolData& p, std::string_view name) {
    u32 offset = static_cast<u32>(p.size());
    const auto* data = reinterpret_cast<const std::byte*>(name.data());
    p.insert(p.end(), data, data + name.size());
    return {offset, static_cast<u16>(name.size())};
}
u32 TregHiveSerializer::iPoolAppendBytes(PoolData& p, std::span<const std::byte> bytes) {
    u32 offset = static_cast<u32>(p.size());
    p.insert(p.end(), reinterpret_cast<const std::byte*>(bytes.data()),
             reinterpret_cast<const std::byte*>(bytes.data() + bytes.size_bytes()));
    return offset;
}

TregAttrRecord TregHiveSerializer::iRecordAttributes(const Attr& attr, PoolData& pool) {
    // dbgprintf("TregDataAttr:%u\n", (u8)attr.getPayload().index()); //why is this 12 ???
    TregAttrRecord attrRec{};
    auto [nameOff, nameLen] = iPoolAppendName(pool, attr.getName());
    attrRec.attrNamePoolOffset = nameOff;
    attrRec.attrNameLength = nameLen;
    attrRec.datatype = attr.getAttrDataType();
    auto payload = attr.getPayload();

    std::visit(
        [&](const auto& value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, std::string> ||
                          std::is_same_v<T, std::vector<std::byte>>) {
                u32 dataOff = iPoolAppendBytes(
                    pool, {reinterpret_cast<const std::byte*>(value.data()), value.size()});
                TregAttrRecord::PayloadPoolData p;
                p.payloadPoolOffset = dataOff;
                p.payloadSize = static_cast<u32>(value.size());
                attrRec.payload = p;
                //   dbgprintf("larlsadlasd\n\n\n\n\n");
            } else {
                TregAttrRecord::TrivialData tData;
                std::memcpy(tData.data(), &value, sizeof(value));
                attrRec.payload = tData;
            }
        },
        attr.getPayload());

    return attrRec;
}
//okay so attr payload is passsed wrongly
TregValueRecord TregHiveSerializer::iRecordValues(const Value& val, PoolData& pool,
                                                  std::vector<TregAttrRecord>& attrRec) {
    TregValueRecord valRec{};
    auto [nameOffset, nameLen] = iPoolAppendName(pool, val.getName());
    valRec.valNameLength = nameLen;
    valRec.valNameOffset = nameOffset;
    std::vector<const Attr*> sortedAttrs = iSortByName(val.getAttrs());
    valRec.attrFirstIndex = static_cast<u32>(attrRec.size());
    valRec.attrCount = static_cast<u16>(sortedAttrs.size());

    for (const Attr* a : sortedAttrs) {
        //  dbgprintf("%u\n", (u8)a->getAttrDataType());
        dbgprintf("BeforePassTregDataAttr:%u\n", (u8)a->getPayload().index()); //why is this 12 ???
        attrRec.push_back(iRecordAttributes(*a, pool));
    }
    return valRec;
}

std::expected<std::vector<std::byte>, Trd::Err::TrdError>
TregHiveSerializer::build(const Trd::Key& root) {
    auto keys = iRecordKeys(root);

    std::vector<TregKeyRecord> keyRec(keys.size());
    std::vector<TregValueRecord> valRec;
    std::vector<TregAttrRecord> attrRec;
    PoolData pool;
    for (size_t i = 0; i < keys.size(); ++i) {
        const Key& k = *keys[i].key;
        auto [nameOff, nameLen] = iPoolAppendName(pool, k.getName());
        keyRec[i].keyNameLength = nameLen;
        keyRec[i].keyNameOffset = nameOff;
        keyRec[i].childKeysCount = keys[i].childCount;
        keyRec[i].firstChildKeyIndex = keys[i].childStart;

        std::vector<const Value*> sortedVal = iSortByName(k.getValues());
        keyRec[i].firstValIndex = static_cast<u32>(valRec.size());
        keyRec[i].valueCount = static_cast<u16>(sortedVal.size());
        for (const auto* v : sortedVal) {
            //  dbgprintf("XPassTregDataAttr:%u\n", (u8)v->getAttrs().at(0).getPayload().index());
            valRec.push_back(iRecordValues(*v, pool, attrRec));
        }
    }
    return serializeData(keyRec, valRec, attrRec, pool);
}

std::vector<std::byte> TregHiveSerializer::serializeData(const std::vector<TregKeyRecord>& keys,
                                                         const std::vector<TregValueRecord>& val,
                                                         const std::vector<TregAttrRecord>& attr,
                                                         const PoolData& pool) {
    const auto poolSize = pool.size() * sizeof(std::byte);
    const u64 bodySize = (keys.size() * TregKeyRecord::keyRecordSize()) +
        (val.size() * TregValueRecord::valueRecordSize() +
         (attr.size() * TregAttrRecord::attrRecordSize())) +
        (poolSize);

    TregHeader hdr;
    hdr.magic = Trd::Consts::Treg::TREG_MAGIC;
    hdr.keyEntriesCount = static_cast<u32>(keys.size());
    hdr.valEntriesCount = static_cast<u32>(val.size());
    hdr.attrEntriesCount = static_cast<u32>(attr.size());
    hdr.poolSize = poolSize;
    hdr._reserved1 = 0;

    std::vector<std::byte> bodyData;
    bodyData.reserve(bodySize);
    //serializeEntity(hdr, bodyData);
    serializeEntity(keys, bodyData);
    serializeEntity(val, bodyData);
    serializeEntity(attr, bodyData);
    bodyData.append_range(std::move(pool));

    Impl::Crc32Gen crcGenerator;
    crcGenerator.addData(bodyData);
    dbgprintf("TREG_CKSUM: 0x%X\nexpected:size: %uB\n", crcGenerator.getCrc32(),
              bodySize + TregHeader::headerSize());
    hdr.checksum = crcGenerator.getCrc32();
    hdr.dbgInfoPrint();
    auto hdrOpt = hdr.serialize();
    if (!hdrOpt)
        throw std::runtime_error("theader serialization failed");
    auto outData = hdrOpt.value();
    outData.append_range(std::move(bodyData));

    return outData;

    //we can only now add the header at begging because the checksum doesnt calculate the header

    /*
    for (const auto& k : keys) {
        const auto serializedK = k.serialize();
        if (!serializedK)
            throw std::runtime_error("failed to serialize tkey");
        bodyData.append_range(std::move(serializedK.value()));
    }
   */
}