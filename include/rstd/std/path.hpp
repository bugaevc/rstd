#pragma once

#include <rstd/core/slice.hpp>

namespace rstd {
namespace std {
namespace path {

class Path {
private:
    // TODO: This should wrap an OStr, not a byte slice
    core::slice::Slice<u8> bytes;

public:
    template<usize len>
    constexpr Path(const char (&arr)[len]) noexcept
        : bytes((const u8 (&)[len - 1]) arr)
    { }

    template<typename F>
    auto run_with_cstr(F f) const {
        char *ptr = (char *) __builtin_malloc(bytes.len() + 1);
        __builtin_memcpy(ptr, bytes.as_ptr(), bytes.len());
        ptr[bytes.len()] = 0;
        auto res = f(ptr);
        __builtin_free(ptr);
        return res;
    }
};

}
}
}
