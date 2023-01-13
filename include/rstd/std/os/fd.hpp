#pragma once

namespace rstd {
namespace std {
namespace os {
namespace fd {

typedef int RawFd;

class OwnedFd {
private:
    RawFd fd;

public:
    OwnedFd(RawFd fd)
        : fd(fd)
    { }

    ~OwnedFd();
};

}
}
}
}
