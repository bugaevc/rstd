#pragma once

#include <rstd/core/panicking.hpp>

#define assert_eq(l, r) do { \
    auto __l = (l); \
    auto __r = (r); \
    if (__l != __r) { \
        rstd::core::panicking::panic(); \
    } \
} while (false)

#define assert_neq(l, r) do { \
    auto __l = (l); \
    auto __r = (r); \
    if (__l == __r) { \
        rstd::core::panicking::panic(); \
    } \
} while (false)

#define try(res) ({ \
    auto __res = (res); \
    if (__res.is_err()) { \
        return Err(__res.unwrap_err()); \
    } \
    __res.unwrap(); \
})

#ifndef __has_cpp_attribute
#define __has_cpp_attribute(x) 0
#endif

#if __has_cpp_attribute(nodiscard)
#define must_use [[nodiscard]]
#else
#define must_use
#endif
