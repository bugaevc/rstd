#pragma once

#include <rstd/core/primitive.hpp>
#include <rstd/core/slice.hpp>
#include <rstd/core/mem/maybe-uninit.hpp>

namespace rstd {
namespace alloc {
namespace vec {

template<typename T>
class Vec {
private:
    core::slice::SliceMut<core::mem::MaybeUninit<T>> mem;
    usize length { 0 };

public:
    Vec()
        : mem(core::slice::SliceMut<core::mem::MaybeUninit<T>>::from_raw_parts(nullptr, 0))
    { }

    ~Vec() {
        for (usize i = 0; i < length; i++) {
            mem[i].destruct();
        }
        __builtin_free(mem.as_ptr());
    }

    static Vec with_capacity(usize capacity) {
        Vec v;
        auto *allocation = (core::mem::MaybeUninit<T> *) __builtin_malloc(capacity * sizeof(T));
        if (allocation == nullptr) {
            panic();
        }
        v.mem = core::slice::SliceMut<core::mem::MaybeUninit<T>>::from_raw_parts(allocation, capacity);
        return v;
    }

    usize len() const {
        return length;
    }

    usize capacity() const {
        return mem.len();
    }

    void reserve(usize additional) {
        if (capacity() >= length + additional) {
            return;
        }
        usize new_capacity = core::next_power_of_two(length + additional);
        auto *allocation = (core::mem::MaybeUninit<T> *) __builtin_malloc(new_capacity * sizeof(T));
        if (allocation == nullptr) {
            panic();
        }
        auto newmem = core::slice::SliceMut<core::mem::MaybeUninit<T>>::from_raw_parts(allocation, new_capacity);
        try {
            for (usize i = 0; i < length; i++) {
                newmem[i].construct(core::cxxstd::move(mem[i].assume_init()));
            }
        } catch (...) {
            __builtin_free(allocation);
            throw;
        }
        for (usize i = 0; i < length; i++) {
            mem[i].destruct();
        }
        __builtin_free(mem.as_ptr());
        mem = newmem;
    }

    void push(T &&item) {
        reserve(1);
        mem[length++].construct(core::cxxstd::forward<T>(item));
    }

    void clear() {
        usize old_len = length;
        length = 0;
        for (usize i = 0; i < old_len; i++) {
            mem[i].destruct();
        }
    }

    void set_len(usize new_len) {
        length = new_len;
    }

    const T *as_ptr() const {
        return (const T *) mem.as_ptr();
    }

    T *as_ptr() {
        return (T *) mem.as_ptr();
    }

    operator core::slice::Slice<T>() const {
        return core::slice::Slice<T>::from_raw_parts(as_ptr(), length);
    }

    operator core::slice::SliceMut<T>() {
        return core::slice::SliceMut<T>::from_raw_parts(as_ptr(), length);
    }

    core::slice::Iter<T> iter() const {
        return ((core::slice::Slice<T>) *this).iter();
    }

    core::slice::IterMut<T> iter_mut() {
        return ((core::slice::SliceMut<T>) *this).iter_mut();
    }

    template<typename I>
    static Vec from_iter(I iter) {
        Vec v;
        for (T &item : iter) {
            v.push(core::cxxstd::move(item));
        }
        return v;
    }

    const T &operator [](usize index) const {
        return ((core::slice::Slice<T>) *this)[index];
    }

    T &operator [](usize index) {
        return ((core::slice::SliceMut<T>) *this)[index];
    }

    core::slice::Slice<T> operator [](core::ops::RangeFrom<usize> index) const {
        return ((core::slice::Slice<T>) *this)[index];
    }

    core::slice::SliceMut<T> operator [](core::ops::RangeFrom<usize> index) {
        return ((core::slice::SliceMut<T>) *this)[index];
    }
};

}
}

using alloc::vec::Vec;
}
