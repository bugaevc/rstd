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
    usize length { 0 };
    SliceMut<core::mem::MaybeUninit<T>> mem;

    static SliceMut<core::mem::MaybeUninit<T>> empty_mem() {
        return SliceMut<core::mem::MaybeUninit<T>>::empty();
    }

    static SliceMut<core::mem::MaybeUninit<T>> realloc_mem(SliceMut<core::mem::MaybeUninit<T>> old, usize count) {
        auto *alloc = (core::mem::MaybeUninit<T> *) __builtin_realloc((void *) old.as_ptr(), count * sizeof(T));
        if (alloc == nullptr) {
            panic();
        }
        return SliceMut<core::mem::MaybeUninit<T>>::from_raw_parts(alloc, count);
    }

public:
    Vec()
        : mem(empty_mem())
    { }

    ~Vec() {
        for (usize i = 0; i < length; i++) {
            mem[i].destruct();
        }
        __builtin_free(mem.as_ptr());
    }

    Vec(const Vec &other)
        : length(other.length)
    {
        mem = realloc_mem(empty_mem(), length);
        try {
            for (usize i = 0; i < length; i++) {
                mem[i].construct(other[i]);
            }
        } catch (...) {
            __builtin_free(mem.as_ptr());
            throw;
        }
    }

    Vec(Vec &&other)
        : length(other.length)
        , mem(other.mem)
    {
        other.length = 0;
        other.mem = empty_mem();
    }

    Vec &operator = (const Vec &other) {
        clear();
        if (capacity() < other.length) {
            mem = realloc_mem(mem.len(), other.length);
        }
        for (usize i = 0; i < other.length; i++) {
            mem[i].construct(other[i]);
        }
        length = other.length;
        return *this;
    }

    Vec &operator = (Vec &&other) {
        clear();
        length = other.length;
        mem = other.mem;
        other.length = 0;
        other.mem = empty_mem();
    }

    static Vec with_capacity(usize capacity) {
        Vec v;
        v.mem = realloc_mem(empty_mem(), capacity);
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
        auto newmem = realloc_mem(empty_mem(), new_capacity);
        try {
            for (usize i = 0; i < length; i++) {
                newmem[i].construct((T &&) mem[i].assume_init());
            }
        } catch (...) {
            __builtin_free(newmem.as_ptr());
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

    operator Slice<T>() const {
        return Slice<T>::from_raw_parts(as_ptr(), length);
    }

    operator SliceMut<T>() {
        return SliceMut<T>::from_raw_parts(as_ptr(), length);
    }

    core::slice::Iter<T> iter() const {
        return ((Slice<T>) *this).iter();
    }

    core::slice::IterMut<T> iter_mut() {
        return ((SliceMut<T>) *this).iter_mut();
    }

    template<typename I>
    static Vec from_iter(I iter) {
        Vec v;
        for (T &item : iter) {
            v.push((T &&) item);
        }
        return v;
    }

    const T &operator [](usize index) const {
        return ((Slice<T>) *this)[index];
    }

    T &operator [](usize index) {
        return ((SliceMut<T>) *this)[index];
    }

    Slice<T> operator [](core::ops::RangeFrom<usize> index) const {
        return ((Slice<T>) *this)[index];
    }

    SliceMut<T> operator [](core::ops::RangeFrom<usize> index) {
        return ((SliceMut<T>) *this)[index];
    }
};

}
}

using alloc::vec::Vec;
}
