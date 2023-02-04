#include <rstd/core/tuple.hpp>
#include <rstd/std/fs.hpp>

using namespace rstd;

extern "C" int printf(const char *format, ...);

int main() {
    Tuple<i8, u32> tuple { 3, 42 };
    printf("%d, %d\n", tuple.get<0>(), tuple.get<1>());
    tuple.get<1>() = 35;
    printf("%d, %d\n", tuple.get<0>(), tuple.get<1>());
}
