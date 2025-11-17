#pragma once
#include <stdint.h>
#include <stddef.h>
#include <optional>
#include <vector>
#include "datatypes.h"
namespace LibTrident::PkgIO {
    struct SerializableData{
        virtual size_t Sizeof() const = 0;
        virtual ~SerializableData() = default;
        virtual std::optional<std::vector<u8>> Serialize() const = 0;
        virtual bool Deserialize(const std::vector<u8>& dataIn) = 0;
        //optional
        virtual std::optional<u32> ChecksumCRC32() const {
            return std::nullopt;
        }

    };
}