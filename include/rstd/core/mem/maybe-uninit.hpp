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
    constexpr MaybeUninit() noexcept {
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

    ~MaybeUninit() noexcept { }

    MaybeUninit(MaybeUninit&) = delete;
    MaybeUninit(MaybeUninit&&) = delete;

    u8 *as_bytes() noexcept {
        return (u8 *) &value;
    }

    const u8 *as_bytes() const noexcept {
        return (const u8 *) &value;
    }

    template<typename... Args>
    T &construct(Args &&...args) {
        return *new(&value) T(cxxstd::forward<Args>(args)...);
    }

    void destruct() {
        value.~T();
    }

    T &assume_init() noexcept {
        return value;
    }

    const T &assume_init() const noexcept {
        return value;
    }
};

template<typename T>
union MaybeUninit<T &> {
private:
    T *ptr;
public:
    constexpr MaybeUninit() noexcept { }
    constexpr MaybeUninit(T &ref)
        : ptr(&ref) { }
    ~MaybeUninit() noexcept { }

    MaybeUninit(MaybeUninit&) = delete;
    MaybeUninit(MaybeUninit&&) = delete;

    u8 *as_bytes() noexcept {
        return (u8 *) &ptr;
    }

    const u8 *as_bytes() const noexcept {
        return (const u8 *) &ptr;
    }

    T &construct(T &ref) noexcept {
        ptr = &ref;
        return ref;
    }

    constexpr void destruct() const noexcept { }

    T &assume_init() noexcept {
        return *ptr;
    }

    const T &assume_init() const noexcept {
        return *ptr;
    }
};

}
}
}
