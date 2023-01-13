#include <rstd/core/slice.hpp>
#include <rstd/core/macros.hpp>
using namespace rstd;

using core::slice::Slice;

extern "C" int printf(const char *format, ...);

int main() {
    i32 arr[] = { 35, 42, 411 };
    Slice<i32> slice = arr;
    assert_eq(slice.len(), 3ul);
    assert_eq(slice[0], 35);
    assert_eq(slice[1], 42);

    assert_eq(slice.get(0).unwrap(), 35);

    slice.iter().map([](i32 x) {
        return (i64) x;
    }).for_each([](i64 v) {
        printf("for_each got %li\n", v);
    });

    auto is_odd = [](i32 v) { return v % 2 == 1; };

    for (auto i : slice.iter().filter(is_odd)) {
        printf("Loop got %i\n", i);
    }
}
