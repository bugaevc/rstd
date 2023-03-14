#pragma once

#include <rstd/core/cxxstd.hpp>

namespace rstd {
namespace core {
namespace mem {

template<typename T>
union MaybeUninit {
private:
    T value;
public:
    constexpr MaybeUninit() {
        static_assert(sizeof(MaybeUninit) == sizeof(T));
        static_assert(alignof(MaybeUninit) == alignof(T));
    }

    constexpr MaybeUninit(T &&value)
        : value((T &&) value)
    {
        static_assert(sizeof(MaybeUninit) == sizeof(T));
        static_assert(alignof(MaybeUninit) == alignof(T));
    }

    constexpr MaybeUninit(const T &value)
        : value(value)
    {
        static_assert(sizeof(MaybeUninit) == sizeof(T));
        static_assert(alignof(MaybeUninit) == alignof(T));
    }

    ~MaybeUninit() { }

    MaybeUninit(MaybeUninit&) = delete;
    MaybeUninit(MaybeUninit&&) = delete;

    u8 *as_bytes() {
        return (u8 *) &value;
    }

    const u8 *as_bytes() const {
        return (const u8 *) &value;
    }

    template<typename... Args>
    T &construct(Args &&...args) {
        return *new(&value) T(cxxstd::forward<Args>(args)...);
    }

    void destruct() {
        value.~T();
    }

    T &assume_init() {
        return value;
    }

    const T &assume_init() const {
        return value;
    }
};

template<typename T>
union MaybeUninit<T &> {
private:
    T *ptr;
public:
    MaybeUninit() { }
    MaybeUninit(T &ref)
        : ptr(&ref) { }
    ~MaybeUninit() { }

    MaybeUninit(MaybeUninit&) = delete;
    MaybeUninit(MaybeUninit&&) = delete;

    u8 *as_bytes() {
        return (u8 *) &ptr;
    }

    const u8 *as_bytes() const {
        return (const u8 *) &ptr;
    }

    T &construct(T &ref) {
        ptr = &ref;
        return ref;
    }

    void destruct() { }

    T &assume_init() {
        return *ptr;
    }

    const T &assume_init() const {
        return *ptr;
    }
};

}
}
}
