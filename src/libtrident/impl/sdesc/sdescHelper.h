#pragma once
#include <span>
#include "datatypes.h"
namespace Trd::Impl {
    inline bool IResvFieldCheck(std::span<const Trd::u8> reserved) {
        for (const auto& x : reserved) {
            if (x != 0)
                return false;
        }
        return true;
    }
};