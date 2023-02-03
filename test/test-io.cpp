#include <rstd/std/fs.hpp>

using namespace rstd;
using rstd::std::fs::File;
using rstd::std::io::BufReader;
using rstd::std::io::BufWriter;
using rstd::std::io::Stdout;
using rstd::std::io::stdout;

rstd::std::io::Result<UnitType> try_main() {
    File file = try(File::open("/etc/fstab"));
    BufReader<File> br { core::cxxstd::move(file) };
    BufWriter<Stdout> bw { stdout() };
    Vec<u8> line;
    while (true) {
        usize res = try(br.read_line(line));
        if (res == 0) {
            break;
        }
        try(bw.write_all(line));
        line.clear();
    }
    try(bw.flush());
    return Ok(Unit);
}

int main() {
    rstd::std::io::Result<UnitType> res = try_main();
    return res.is_ok() ? 0 : 1;
}
