#include <rstd/core/str.hpp>

namespace rstd {
namespace core {
namespace str {

Option<str> Split::next() {
    if (data.is_empty()) {
        return None;
    }
    // Emulate strchrnul().
    const u8 *p = (const u8 *) __builtin_memchr(data.as_ptr(), split_byte, data.len());
    usize len = p ? p - data.as_ptr() : data.len();
    tuple::Tuple<Slice<u8>, Slice<u8>> t = data.split_at(len);
    data = t.get<1>();
    if (p) {
        data = data[ops::RangeFrom<usize>(1)];
    }
    // TODO: Can this be unchecked?
    str s = from_utf8(t.get<0>()).unwrap();
    return Some<str>(s);
}

Split str::split(u8 split_byte) const {
    return Split { inner, split_byte };
}

Lines str::lines() const {
    return split('\n');
}

}
}
}
