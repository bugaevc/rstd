#include <rstd/std/fs.hpp>
#include <unistd.h>
#include <fcntl.h>

namespace rstd {
namespace std {
namespace fs {

io::Result<File> File::open(path::Path path) {
    int fd = path.run_with_cstr([](const char *p) {
        return ::open(p, O_RDONLY | O_CLOEXEC);
    });
    if (fd < 0)
        return Err<File, io::Error>(io::Error::last_os_error());
    return Ok<File, io::Error>(File(os::fd::OwnedFd(fd)));
}

io::Result<usize> File::read(core::slice::SliceMut<u8> buf) {
    int rc = ::read(fd.as_raw_fd(), buf.as_ptr(), buf.len());
    if (rc < 0)
        return Err<usize, io::Error>(io::Error::last_os_error());
    return Ok<usize, io::Error>(rc);
}

}
}
}
