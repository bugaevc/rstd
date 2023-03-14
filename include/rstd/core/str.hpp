#pragma once

#include <rstd/core/cxxstd.hpp>
#include <rstd/core/slice.hpp>
#include <rstd/core/tuple.hpp>

namespace rstd {
namespace core {
namespace str {

class str;

class Utf8Error {
private:
    usize valid_up_to_;
    Utf8Error(usize valid_up_to_)
        : valid_up_to_(valid_up_to_)
    { }
    friend Result<str, Utf8Error> from_utf8(Slice<u8>);

public:
    usize valid_up_to() const {
        return valid_up_to_;
    }
};

Result<str, Utf8Error> from_utf8(Slice<u8> bytes);

namespace __internal {

constexpr usize utf8_byte_classify(u8 byte) {
    return ((byte & 0x80) == 0) ? ((byte == 0) ? 30 : 0) : (
        ((byte & 0xe0) == 0xc0) ? 1 : (
            ((byte & 0xf0) == 0xe0) ? 2 : (
                ((byte & 0xf8) == 0xf0) ? 3 : (
                    ((byte & 0xc0) == 0x80) ? 10 : 20
                )
            )
        )
    );
}

constexpr usize saturating_inc(usize value) {
    return (value == SIZE_MAX) ? SIZE_MAX : (value + 1);
}

constexpr usize utf8_valid_up_to(const u8 *data, usize data_len, usize cont_bytes) {
    return (data_len == 0) ? (
        (cont_bytes == 0) ? SIZE_MAX : 0
    ) : (
        (cont_bytes == 0) ? (
            (utf8_byte_classify(data[0]) < 10) ? saturating_inc(utf8_valid_up_to(data + 1, data_len - 1, utf8_byte_classify(data[0]))) : 0
        ) : (
            (utf8_byte_classify(data[0]) == 10) ? saturating_inc(utf8_valid_up_to(data + 1, data_len - 1, cont_bytes - 1)) : 0
        )
    );
}

}

class Split : public iter::Iterator<Split, str> {
private:
    Slice<u8> data;
    // TODO: Pattern/Searcher
    u8 split_byte;

    Split(Slice<u8> data, u8 split_byte)
        : data(data)
        , split_byte(split_byte)
    { }

    friend class str;

public:
    Option<str> next();
};

using Lines = Split;
using Bytes = slice::Iter<u8>;

class str {
private:
    Slice<u8> inner;
    str(Slice<u8> inner) : inner(inner) { }
    friend str from_utf8_unchecked(Slice<u8> bytes);

public:
    template<usize N>
    str(const char (&arr)[N])
        : inner(Slice<u8>::from_raw_parts((const u8 *) arr, N - 1))
    {
        assert_eq(arr[N - 1], 0);
        assert_eq(__internal::utf8_valid_up_to((const u8 *) arr, N - 1, 0), SIZE_MAX);
    }

    usize len() const {
        return inner.len();
    }

    bool is_empty() const {
        return inner.is_empty();
    }

    Slice<u8> as_bytes() const {
        return inner;
    }

    const u8 *as_ptr() const {
        return inner.as_ptr();
    }

    bool is_ascii() const {
        return bytes().all(core::is_ascii);
    }

    Bytes bytes() const {
        return inner.iter();
    }

    Split split(u8 split_byte) const;
    Lines lines() const;
};

str from_utf8_unchecked(Slice<u8> bytes) {
    return str(bytes);
}

Result<str, Utf8Error> from_utf8(Slice<u8> bytes) {
    usize valid_up_to = __internal::utf8_valid_up_to(bytes.as_ptr(), bytes.len(), 0);
    if (valid_up_to == SIZE_MAX) {
        return Ok(from_utf8_unchecked(bytes));
    } else {
        return Err(Utf8Error(valid_up_to));
    }
}

}
}

using core::str::str;
}
