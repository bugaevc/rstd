#include <rstd/alloc/vec.hpp>
#include <rstd/core/macros.hpp>
using namespace rstd;

extern "C" int printf(const char *format, ...);

int main() {
    Vec<i32> v;
    v.push(35);
    v.push(42);

    Slice<i32> sl = v;
    for (auto item : sl.iter()) {
        printf("Found %i\n", item);
    }

    Vec<u32> nums = v.iter().map([](i32 a) {
        return (u32) a + 1;
    }).collect<Vec<u32>>();

    for (u32 n : nums.iter()) {
        printf("Got %u\n", n);
    }
}
