#pragma once
#include "trderr.h"
#include "datatypes.h"
#include "trdconsts.h"
#include "ccattribs.h"
#include "serdatacommon.h"
#include "binarySerializer.h"

namespace Trd::Consts::Treg {
    inline constexpr u32 TREG_MAGIC = 0x52484452; //RHDR (BE)
};

namespace Trd::Impl {
    namespace RegFlags {
        enum Flag : u16 {
            Clear = 0
        };
    };

    struct TregHeader : Impl::SerializableData {
        u32 magic{Consts::Treg::TREG_MAGIC};
        RegFlags::Flag flags{RegFlags::Clear};
        file_offset regRootOffset;
        u64 regSizeTotal;
        u64 _reserved;
        u16 _reserved1;

        constexpr u64 size() const override {
            return Impl::BinarySerializer::elementSize(magic, flags, regRootOffset, regSizeTotal,
                                                       _reserved, _reserved1);
        }
        std::optional<std::vector<std::byte>> serialize() const override;

        bool deserialize(const std::vector<std::byte>& dataIn) override;

        std::optional<u32> checksumCRC32() const;
    };

};