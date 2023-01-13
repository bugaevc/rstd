#include <rstd/core/panicking.hpp>

#define assert_eq(l, r) do { \
    auto __l = (l); \
    auto __r = (r); \
    if (__l != __r) { \
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
