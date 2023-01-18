#pragma once

#include <stdint.h>
#include <sys/types.h>

typedef int8_t i8;
typedef uint8_t u8;
typedef int16_t i16;
typedef uint16_t u16;
typedef int32_t i32;
typedef uint32_t u32;
typedef int64_t i64;
typedef uint64_t u64;

typedef ssize_t isize;
typedef size_t usize;

typedef float f32;
typedef double f64;

namespace rstd {
namespace core {

constexpr usize next_power_of_two(usize self) {
    if (self <= 1) {
        return 1;
    }
    int clzl = __builtin_clzl(self);
    usize np2 = 1ul << (sizeof(usize) - clzl);
    if (self * 2 == np2) {
        return self;
    }
    return np2;
}
}
}
