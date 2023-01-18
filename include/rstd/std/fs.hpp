#pragma once

#include <rstd/core/cxxstd.hpp>
#include <rstd/std/os/fd.hpp>
#include <rstd/std/path.hpp>
#include <rstd/std/io.hpp>

namespace rstd {
namespace std {
namespace fs {

class File : public io::Read<File> {
private:
    os::fd::OwnedFd fd;

public:
    explicit File(os::fd::OwnedFd &&fd)
        : fd(core::cxxstd::move(fd))
    { }

    File(const File &) = delete;

    File(File &&other)
        : fd(core::cxxstd::move(other.fd))
    { }

    static io::Result<File> open(path::Path path);

    io::Result<usize> read(core::slice::SliceMut<u8> buf);
};

}
}
}
