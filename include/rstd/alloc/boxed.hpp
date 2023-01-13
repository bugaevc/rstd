#pragma once

#include <rstd/core/cxxstd.hpp>

namespace rstd {
namespace alloc {
namespace boxed {

template<typename T>
class Box {
private:
    T *ptr;

    Box(T *ptr)
        : ptr(ptr)
    { }

public:
    template<typename... Args>
    explicit Box(Args &&...args) {
        ptr = __builtin_malloc(sizeof(T));
        new(ptr) T(cxxstd::forward<Args>(args)...);
    }

    ~Box() {
        ptr->~T();
        __builtin_free(ptr);
    }

    Box(Box &&other) = delete;
    Box &operator =(Box &&other) = delete;

    T &operator *() {
        return *ptr;
    }

    const T &operator *() const {
        return *ptr;
    }

    T &operator ->() {
        return *ptr;
    }

    const T &operator ->() const {
        return *ptr;
    }

    static Box from_raw(T *ptr) {
        return Box(ptr);
    }
};

}
}

using alloc::boxed::Box;
}
