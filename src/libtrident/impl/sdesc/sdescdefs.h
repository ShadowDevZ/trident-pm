#pragma once
#include "trderr.h"
#include "datatypes.h"
#include "trdconsts.h"
#include "ccattribs.h"
#include "serdatacommon.h"
#include "binarySerializer.h"

namespace Trd::Impl {

    //removed feature for now, will be moved into separate tmp file.
    /*allows smooth control of multiple different processes accessing the same file resource
    and the same part of library checking. Note if status flag is not clear then the
    other process MUST NOT perform any IO operation which could
    alter the file manipulation by the host program under any circumstances, this check should be done
    using some OS specific function by other process accessing this resource not as stored field in case
    of crash the package would be bricked
    permamently
    namespace SectionStatusFlag {
        enum Flag : u16 {
            Clear = 0,
            ReadLockHeader = 1 << 1, // header write operation in progress reading not advised
            WriteLockHeader = 1 << 2,

            ReadLockDynamic = 1 << 3,
            WriteLockDynamic = 1 << 4,

            ReadLockTreg = 1 << 5,
            WriteLocTreg = 1 << 6,

            LockAll = 1 << 15
        };
    };
*/
    struct SD_TBLENTRY {
        uint64_t offset{0};
        uint64_t size{0};
        u8bool::type available{u8bool::type::False};
    };
    struct TRD_SD_UPDATEFIELD {
        //std::optional<SectionStatusFlag::Flag> sectionStatusCode{SectionStatusFlag::Clear};

        std::optional<SD_TBLENTRY> tblDynamic;
        std::optional<SD_TBLENTRY> tblRegistry;
    };

    struct TRD_SECTION_DESCRIPTOR : Impl::SerializableData {
        u32 crc;
        /*status code bitflags to determine what part of offsets contain valid offset
        for example if dynamic offset flag is cleared and some resizing of .dtbl occurs
        because we are adding a new table. */

        // u16
        //SectionStatusFlag::Flag sectionStatusCode{SectionStatusFlag::Clear};

        SD_TBLENTRY tblDynamic{};
        SD_TBLENTRY tblRegistry{};
        //u32 _reserved2[2];
        std::array<u8, 22> _reserved2{};
        u16 _reserved1{0};
        u8bool::type sdReady{u8bool::type::False};
        // why simply not use const here ? using const prevents struct assigning as const
        // cannot be assigned
        u8 idByte{Consts::SD::TRD_SD_IDBYTE};

        constexpr u64 size() const override {
            return Impl::BinarySerializer::elementSize(
                crc, sdReady, tblDynamic.offset, tblDynamic.size, tblDynamic.available,
                tblRegistry.offset, tblRegistry.size, tblRegistry.available, idByte, _reserved2,
                _reserved1);
        }
        // again in code we shouldnt even bother what is idByte outside of validating external SD struct
        std::optional<std::vector<u8>> serialize() const override;

        bool deserialize(const std::vector<u8>& dataIn) override;

        std::optional<u32> checksumCRC32() const;

        bool operator==(const TRD_SECTION_DESCRIPTOR& other) const {
            return crc == other.crc && tblDynamic.offset == other.tblDynamic.offset &&
                tblDynamic.size == other.tblDynamic.size &&
                tblDynamic.available == other.tblDynamic.available &&
                tblRegistry.offset == other.tblRegistry.offset &&
                tblRegistry.size == other.tblRegistry.size &&
                tblRegistry.available == other.tblRegistry.available &&
                _reserved2 == other._reserved2 && sdReady == other.sdReady;
        }
    };
};