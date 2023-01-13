#include <rstd/std/os/fd.hpp>
#include <unistd.h>

namespace rstd {
namespace std {
namespace os {
namespace fd {

OwnedFd::~OwnedFd() {
    close(fd);
}

}
}
}
}
