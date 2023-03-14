#pragma once

#include <rstd/core/slice.hpp>
#include <rstd/core/cxxstd.hpp>

namespace rstd {
namespace std {
namespace path {

class Path {
private:
    // TODO: This should wrap an OStr, not a byte slice
    Slice<u8> bytes;

public:
    template<usize len>
    constexpr Path(const char (&arr)[len]) noexcept
        : bytes((const u8 (&)[len - 1]) arr)
    { }

    template<typename F>
    core::cxxstd::invoke_result_t<F, const char *>
    run_with_cstr(F f) const {
        char *ptr = (char *) __builtin_malloc(bytes.len() + 1);
        __builtin_memcpy(ptr, bytes.as_ptr(), bytes.len());
        ptr[bytes.len()] = 0;
        auto res = f((const char *) ptr);
        __builtin_free(ptr);
        return res;
    }
};

}
}
}
