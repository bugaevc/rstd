#pragma once

#include <rstd/core/primitive.hpp>
#include <rstd/core/panicking.hpp>
#include <rstd/core/option.hpp>
#include <rstd/core/iter.hpp>
#include <rstd/core/ops.hpp>

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

    Slice(const T *data, usize length)
        : data(data), length(length)
    { }

public:
    usize len() const {
        return length;
    }

    bool is_empty() const {
        return length == 0;
    }

    const T *as_ptr() const {
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

    static Slice from_raw_parts(const T *data, usize len) {
        return Slice { data, len };
    }

    template<usize N>
    Slice(const T (&arr)[N])
    {
        data = &arr[0];
        length = N;
    }

    Option<const T &> get(usize index) const {
        if (index >= length) {
            return None;
        } else {
            return Some<const T &>(data[index]);
        }
    }

    Iter<T> iter() const {
        return Iter<T>(*this);
    }
};

template<typename T>
class SliceMut {
private:
    T *data;
    usize length;

    SliceMut(T *data, usize length)
        : data(data), length(length)
    {}

public:
    usize len() const {
        return length;
    }

    bool is_empty() const {
        return length == 0;
    }

    T *as_ptr() const {
        return data;
    }

    operator Slice<T>() const
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

    static SliceMut from_raw_parts(T *data, usize len)
    {
        return SliceMut { data, len };
    }

    template<usize N>
    SliceMut(T (&arr)[N])
    {
        data = &arr[0];
        length = N;
    }

    Iter<T> iter() const {
        return Iter<T>(*this);
    }

    IterMut<T> iter_mut() const {
        return IterMut<T>(*this);
    }
};

template<typename T>
class Iter : public iter::Iterator<Iter<T>, const T &> {
private:
    Slice<T> slice;

public:
    Iter(Slice<T> slice)
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
    IterMut(SliceMut<T> slice)
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
}
