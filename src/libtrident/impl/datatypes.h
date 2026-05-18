#pragma once
#include <cstdint>
#include "ccattribs.h"
#include <concepts>
#include <stdexcept>
namespace Trd {

    //basic datatypes

    using u8 = uint8_t;

    using u16 = uint16_t;
    using u32 = uint32_t;
    using u64 = uint64_t;

    using i8 = int8_t;
    using i16 = int16_t;
    using i32 = int32_t;
    using i64 = int64_t;

    using uint = uint32_t;

    using IO_OpenFlag = uint32_t;

    using foffset_t = uint64_t;

    //used instead of the C++ bool because bool does not have standard size
    //the size could be anywhere from 1 byte, to make things platform independent we have to improvise
    namespace u8bool {
        enum class type : u8 {
            False = 0,
            True = 1
        };
        constexpr type fromUint8(u8 b) {
            if (b != 1 && b != 0)
                throw std::invalid_argument("Type of u8_bool was set to not boolean value");
            return static_cast<type>(b);
        }
        constexpr type fromBool(bool b) noexcept {
            return b ? type::True : type::False;
        }
        constexpr bool toBool(type b) noexcept {
            return static_cast<u8>(b) != 0;
        }
        constexpr u8 toUint8(type b) noexcept {
            return static_cast<u8>(b);
        }

    };

#ifdef EXP_TRY
#error "TRY macro already defined"
#else
//convenient macro that lets us test std::expected if we only care to check if function failed and get error
#define EXP_TRY(expr)                                                                              \
    if (auto _r = (expr); !_r)                                                                     \
    return std::unexpected(_r.error())
#endif
};
