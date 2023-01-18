#include <rstd/std/os/fd.hpp>
#include <unistd.h>

namespace rstd {
namespace std {
namespace os {
namespace fd {

OwnedFd::~OwnedFd() {
    if (fd != -1) {
        close(fd);
    }
}

OwnedFd::OwnedFd(OwnedFd &&other)
    : fd(other.fd)
{
    other.fd = -1;
}

}
}
}
}
