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
            } else {
                TregAttrRecord::TrivialData tData;
                std::memcpy(tData.data(), &value, sizeof(value));
                attrRec.payload = tData;
            }
        },
        attr.getPayload());

    return attrRec;
}

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
        attrRec.push_back(iRecordAttributes(*a, pool));
    }
    return valRec;
}