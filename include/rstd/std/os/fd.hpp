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
    explicit OwnedFd(RawFd fd)
        : fd(fd)
    { }

    OwnedFd(OwnedFd &) = delete;
    OwnedFd(OwnedFd &&);

    ~OwnedFd();

    RawFd as_raw_fd() const {
        return fd;
    }
};

}
}
}
}
