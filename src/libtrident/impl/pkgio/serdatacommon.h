#pragma once
#include <stdint.h>
#include <stddef.h>
#include <optional>
#include <vector>
#include "datatypes.h"
namespace LibTrident::Impl {
    struct SerializableData{
        virtual constexpr size_t size() const = 0;
        virtual ~SerializableData() = default;
        virtual std::optional<std::vector<u8>> serialize() const = 0;
        virtual bool deserialize(const std::vector<u8>& dataIn) = 0;
        //optional
        virtual std::optional<u32> checksumCRC32() const {
            return std::nullopt;
        }

    };
}