#pragma once

#include <rstd/core/primitive.hpp>
#include <rstd/core/panicking.hpp>
#include <rstd/core/option.hpp>
#include <rstd/core/iter.hpp>
#include <rstd/core/ops.hpp>
#include <rstd/core/tuple.hpp>

namespace rstd {
namespace core {
namespace slice {

template<typename T>
class Iter;

template<typename T>
class IterMut;

template<typename T>
class Slice {
private:
    const T *data;
    usize length;

    constexpr Slice(const T *data, usize length) noexcept
        : data(data), length(length)
    { }

public:
    constexpr usize len() const noexcept {
        return length;
    }

    constexpr bool is_empty() const noexcept {
        return length == 0;
    }

    constexpr const T *as_ptr() const noexcept {
        return data;
    }

    const T &operator[](usize index) const {
        if (index >= length) {
            panic();
        }
        return data[index];
    }

    Slice operator[](ops::Range<usize> range) const {
        if (range.end > length || range.start < range.end) {
            panic();
        }
        return Slice { data + range.start, range.end - range.start };
    }

    Slice operator[](ops::RangeFrom<usize> range) const {
        if (range.start > length) {
            panic();
        }
        return Slice { data + range.start, length - range.start };
    }

    static constexpr Slice from_raw_parts(const T *data, usize len) noexcept {
        return Slice { data, len };
    }

    static constexpr Slice empty() noexcept {
        return Slice { nullptr, 0 };
    }

    template<usize N>
    constexpr Slice(const T (&arr)[N]) noexcept
        : data(arr)
        , length(N)
    { }

    Option<const T &> get(usize index) const noexcept {
        if (index >= length) {
            return None;
        } else {
            return Some<const T &>(data[index]);
        }
    }

    constexpr Iter<T> iter() const noexcept {
        return Iter<T>(*this);
    }

    Tuple<Slice, Slice> split_at(usize mid) const {
        if (mid > length) {
            panic();
        }
        Slice first { data, mid };
        Slice second { data + mid, length - mid };
        return Tuple<Slice, Slice>(first, second);
    }

    Option<Tuple<T &, Slice>> split_first() const {
        if (is_empty()) {
            return None;
        }
        Slice tail { data + 1, length - 1 };
        return Some(Tuple<T &, Slice>(data[0], tail));
    }

    Option<Tuple<T &, Slice>> split_last() const {
        if (is_empty()) {
            return None;
        }
        Slice head { data, length - 1 };
        return Some(Tuple<T &, Slice>(data[length - 1], head));
    }
};

template<typename T>
class SliceMut {
private:
    T *data;
    usize length;

    constexpr SliceMut(T *data, usize length) noexcept
        : data(data), length(length)
    {}

public:
    constexpr usize len() const noexcept {
        return length;
    }

    constexpr bool is_empty() const noexcept {
        return length == 0;
    }

    constexpr T *as_ptr() const noexcept {
        return data;
    }

    constexpr operator Slice<T>() const noexcept
    {
        return Slice<T>::from_raw_parts(data, length);
    }

    const T &operator[](usize index) const {
        if (index >= length) {
            panic();
        }
        return data[index];
    }

    T &operator[](usize index) {
        if (index >= length) {
            panic();
        }
        return data[index];
    }

    Slice<T> operator[](ops::Range<usize> range) const {
        if (range.end > length || range.start < range.end) {
            panic();
        }
        return Slice<T> { data + range.start, range.end - range.start };
    }

    Slice<T> operator[](ops::RangeFrom<usize> range) const {
        if (range.start > length) {
            panic();
        }
        return Slice<T> { data + range.start, length - range.start };
    }

    SliceMut operator[](ops::Range<usize> range) {
        if (range.end > length || range.start < range.end) {
            panic();
        }
        return SliceMut { data + range.start, range.end - range.start };
    }

    SliceMut operator[](ops::RangeFrom<usize> range) {
        if (range.start > length) {
            panic();
        }
        return SliceMut { data + range.start, length - range.start };
    }

    constexpr static SliceMut from_raw_parts(T *data, usize len) noexcept
    {
        return SliceMut { data, len };
    }

    constexpr static SliceMut empty() noexcept {
        return SliceMut { nullptr, 0 };
    }

    template<usize N>
    constexpr SliceMut(T (&arr)[N]) noexcept
        : data(arr)
        , length(N)
    { }

    constexpr Iter<T> iter() const noexcept {
        return Iter<T>(*this);
    }

    constexpr IterMut<T> iter_mut() const noexcept {
        return IterMut<T>(*this);
    }
};

template<typename T>
class Iter : public iter::Iterator<Iter<T>, const T &> {
private:
    Slice<T> slice;

public:
    constexpr Iter(Slice<T> slice) noexcept
        : slice(slice)
    { }

    Option<const T &> next() {
        if (slice.is_empty()) {
            return None;
        }
        const T &first = slice[0];
        slice = Slice<T>::from_raw_parts(slice.as_ptr() + 1, slice.len() - 1);
        return Some<const T &>(first);
    }
};

template<typename T>
class IterMut : public iter::Iterator<IterMut<T>, T &> {
private:
    SliceMut<T> slice;

public:
    constexpr IterMut(SliceMut<T> slice) noexcept
        : slice(slice)
    { }

    Option<T &> next() {
        if (slice.is_empty()) {
            return None;
        }
        T &first = slice[0];
        slice = SliceMut<T>::from_raw_parts(slice.as_ptr() + 1, slice.len() - 1);
        return Some(first);
    }
};

}
}

using core::slice::Slice;
using core::slice::SliceMut;
}
