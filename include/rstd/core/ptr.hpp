#pragma once

#include <rstd/core/mem/maybe-uninit.hpp>

namespace rstd {
namespace core {
namespace ptr {

template<typename T>
T read(const T *src) {
    mem::MaybeUninit<T> mu;
    __builtin_memcpy(mu.as_bytes(), src, sizeof(T));
    return mu.assume_init();
}

template<typename T>
void write(T *dst, T &&src) {
    new(dst) T(src);
}

template<typename T>
void destruct_in_place(T *obj) {
    T->~T();
}

}
}
}
