#include <rstd/core/str.hpp>
#include <rstd/core/macros.hpp>

using namespace rstd;

extern "C" int printf(const char *format, ...);

int main() {
    str s = "how\ncan I\ngo on\nfrom day to day";
    for (str line : s.lines()) {
        printf("%lu %.*s\n", line.len(), (int) line.len(), line.as_ptr());
    }
    for (usize len : s.lines().map(core::cxxstd::mem_fn(&str::len))) {
        printf("%lu\n", len);
    }
}
