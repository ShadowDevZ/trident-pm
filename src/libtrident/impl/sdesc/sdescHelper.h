#pragma once
#include <span>
//#include "datatypes.h"
namespace Trd::Impl {
    inline bool IResvFieldCheck(std::span<const std::byte> reserved) {
        for (const auto& x : reserved) {
            if (x != std::byte{0})
                return false;
        }
        return true;
    }
};