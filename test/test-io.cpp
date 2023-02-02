#include <rstd/std/fs.hpp>

using namespace rstd;
using rstd::core::slice::Slice;
using rstd::std::fs::File;
using rstd::std::io::ErrorKind;
using rstd::std::io::BufReader;

extern "C" int printf(const char *format, ...);

void print_str(Slice<u8> str) {
    for (u8 byte : str.iter()) {
        printf("%c", byte);
    }
}

int main() {
    File file = File::open("/etc/fstab").unwrap();
    BufReader<File> br { core::cxxstd::move(file) };
    Vec<u8> line;
    while (true) {
        usize res = br.read_line(line).unwrap();
        if (res == 0) {
            break;
        }
        print_str(line);
        line.clear();
    }
}
