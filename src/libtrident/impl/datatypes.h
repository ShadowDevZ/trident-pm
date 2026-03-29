#pragma once
#include <cstdint>
#include "ccattribs.h"
#include <concepts>
namespace Trd {

//basic datatypes

using u8 = uint8_t;
using u8_bool = u8;


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
template <typename T>
requires std::same_as<T, u8_bool>
constexpr bool u8b_check(T b) {
    return (b == 1 || b == 0);
} 


};
