#pragma once
#include "trderr.h"
#include "datatypes.h"
#include "trdconsts.h"
#include "ccattribs.h"
#include "serdatacommon.h"
#include "binarySerializer.h"
#include <variant>
namespace Trd::Consts::Treg {
    inline constexpr u32 TREG_MAGIC = 0x52444852; //RHDR (BE)
};

namespace Trd {
    namespace RegFlags {
        enum Flag : u16 {
            Clear = 0
        };
    };
    enum class TregAttrDatatype : u8 {
        i8 = 0,
        i16 = 1,
        i32 = 2,
        i64 = 3,
        u8 = 4,
        u16 = 5,
        u32 = 6,
        u64 = 7,
        f32 = 8,
        f64 = 9,
        u8_bool = 10,
        cstr = 11, // null terminated
        binDataLE = 12,
        binDataBE = 13 //not implemented currently
    };

    struct TregHeader : Impl::SerializableData {
        /*
        u32 magic{Consts::Treg::TREG_MAGIC};
        RegFlags::Flag flags{RegFlags::Clear};
        file_offset regRootOffset;
        u64 regSizeTotal;
        u64 _reserved;
        u16 _reserved1;
*/

        u32 magic{Consts::Treg::TREG_MAGIC};
        RegFlags::Flag flags{RegFlags::Clear};
        u32 keyEntriesCount;
        u32 valEntriesCount;
        u32 attrEntriesCount;
        u64 poolSize;
        u32 checksum; //everything after header until the end of treg section
        u16 _reserved1;

        static constexpr int headerSize() {
            return 32;
        }

        constexpr u64 size() const override {
            return Impl::BinarySerializer::elementSize(magic, flags, keyEntriesCount,
                                                       valEntriesCount, attrEntriesCount, poolSize,
                                                       checksum, _reserved1);
        }
        std::optional<std::vector<std::byte>> serialize() const override;

        bool deserialize(const std::vector<std::byte>& dataIn) override;
        //doesnt make sense here, only in final TregObject
        std::optional<Impl::Crc32Gen> checksumCRC32() const;
    };
    //singular key record
    struct TregKeyRecord : Impl::SerializableData {
        u32 keyNameOffset; //offset in flat vector
        u16 keyNameLength;
        u32 firstChildKeyIndex;
        u16 childKeysCount;

        u32 firstValIndex;
        u16 valueCount;
        /*
        sum of all fields in bytes, there is probably a way better way to do this but i want this 
        to be static and i dont want to use magic numbers that make no sense
        */
        static constexpr int keyRecordSize() {
            return 18;
        }
    };

    struct TregValueRecord : Impl::SerializableData {
        u32 valNameOffset;
        u16 valNameLength;
        u32 attrFirstIndex;
        u16 attrCount;
        static constexpr int valueRecordSize() {
            return 12;
        }
    };
    struct TregAttrRecord : Impl::SerializableData {
        u32 attrNamePoolOffset;
        u16 attrNameLength;
        TregAttrDatatype
            dataType; // if dataType < 11 store data in smallData otherwiese point to pool
        //with LargeData; if dataSize smaller than 8 && dataType < 11 pad with 0's until end

        struct PayloadPoolData {
            u32 payloadPoolOffset;
            u32 payloadSize;
        };
        u8 tempPadVal;
        using TrivialData = std::array<std::byte, 8>;
        std::variant<TrivialData, PayloadPoolData> payload;
        static constexpr int attrRecordSize() {
            return 16;
        }
    };

    struct Attr {
        std::string attributeName;
        TregAttrDatatype type;
        std::vector<std::byte> data;

        static Attr u8(std::string n, u8 v) {
            return fillTrivial(std::move(n), TregAttrDatatype::u8, v);
        }
        static Attr u16(std::string n, u16 v) {
            return fillTrivial(std::move(n), TregAttrDatatype::u16, v);
        }
        static Attr u32(std::string n, u32 v) {
            return fillTrivial(std::move(n), TregAttrDatatype::u32, v);
        }
        static Attr u64(std::string n, u64 v) {
            return fillTrivial(std::move(n), TregAttrDatatype::u64, v);
        }
        //fill later
        template <typename T>
        static Attr fillTrivial(std::string name, TregAttrDatatype type, T value,
                                std::endian forceByteOrder = std::endian::native) {

            Impl::BinarySerializer bs(forceByteOrder, true);
            Attr attr;
            attr.attributeName = std::move(name);
            attr.type = type;
            bs.addTrivial(value);
            auto fmt = bs.getFormattedData();
            if (!fmt)
                throw std::runtime_error("Attribute data could not be properly serialized");
            attr.data = fmt.value();

            return attr;
        }
    };

    struct Value {
        std::string value;
        std::vector<Attr> attrs;
    };

    struct Key {
        std::string key;
        std::vector<Key> children;
        std::vector<Value> values;
    };

    static inline bool AttrIsTrivial(TregAttrDatatype type) {
        constexpr int lastTrivialAttrIndex = 10;
        if (static_cast<u8>(type) <= lastTrivialAttrIndex)
            return true;
        return false;
    };

};