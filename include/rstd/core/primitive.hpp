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

class Never {
private:
    Never() {
        __builtin_unreachable();
    }

public:
    Never(const Never &other) {
        __builtin_unreachable();
    }

    Never(Never &&other) {
        __builtin_unreachable();
    }

    ~Never() {
        __builtin_unreachable();
    }

    template<typename T>
    operator T() const {
        __builtin_unreachable();
    }
};

static inline usize next_power_of_two(usize self) {
    if (self <= 1) {
        return 1;
    }
    int clzl = __builtin_clzl(self);
    usize np2 = 1ul << (sizeof(usize) * 8 - clzl);
    if (self * 2 == np2) {
        return self;
    }
    return np2;
}

static inline constexpr bool is_ascii(u8 byte) {
    return byte < 128;
}
}

using core::Never;

}
