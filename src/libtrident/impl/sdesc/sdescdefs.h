#pragma once
#include "trderr.h"
#include "datatypes.h"
#include "trdconsts.h"
#include "ccattribs.h"
#include "serdatacommon.h"
#include "binarySerializer.h"

namespace Trd::Impl {
    /*allows smooth control of multiple different processes accessing the same file resource
    and the same part of library checking. Note if status flag is not clear then the
    other process MUST NOT perform any IO operation which could
    alter the file manipulation by the host program under any circumstances, this check should be done
    using some OS specific function by other process accessing this resource not as stored field in case
    of crash the package would be bricked
    permamently*/
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

    typedef struct {
        std::optional<SectionStatusFlag::Flag> sectionStatusCode{SectionStatusFlag::Clear};
        std::optional<u32> tblCount;
        std::optional<u64> tblDynamicOffset;
        std::optional<u64> tblRegistryOffset;
    } TRD_SD_UPDATEFIELD;

    struct TRD_SECTION_DESCRIPTOR : Impl::SerializableData {
        u32 crc;
        /*status code bitflags to determine what part of offsets contain valid offset
        for example if dynamic offset flag is cleared and some resizing of .dtbl occurs
        because we are adding a new table. */

        // u16
        SectionStatusFlag::Flag sectionStatusCode{SectionStatusFlag::Clear};
        u32 tblCount;
        u32 _reserved1;
        u64 tblDynamicOffset;
        u64 tblRegistryOffset;
        u64 _reserved2;
        u8_bool sdReady;
        // why simply not use const here ? using const prevents struct assigning as const
        // cannot be assigned
        u8 idByte{Consts::SD::TRD_SD_IDBYTE};

        constexpr u64 size() const override {
            return Impl::BinarySerializer::elementSize(crc, sectionStatusCode, sdReady, tblCount,
                                                       tblDynamicOffset, tblRegistryOffset,
                                                       _reserved1, _reserved2, idByte);
        }
        // again in code we shouldnt even bother what is idByte outside of validating external SD struct
        std::optional<std::vector<u8>> serialize() const override;

        bool deserialize(const std::vector<u8>& dataIn) override;

        std::optional<u32> checksumCRC32() const;

        bool operator==(const TRD_SECTION_DESCRIPTOR& other) const {
            return crc == other.crc && sectionStatusCode == other.sectionStatusCode &&
                tblCount == other.tblCount && _reserved1 == other._reserved1 &&
                tblDynamicOffset == other.tblDynamicOffset &&
                tblRegistryOffset == other.tblRegistryOffset && _reserved2 == other._reserved2 &&
                sdReady == other.sdReady;
        }
    };
};