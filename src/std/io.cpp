#include <rstd/std/io.hpp>

#include <errno.h>

namespace rstd {
namespace std {
namespace io {

Error Error::from_raw_os_error(i32 code) {
    switch (code) {
    case ENOENT:
        return ErrorKind::NotFound;
    case EPERM:
        return ErrorKind::PermissionDenied;
    // TODO
    case ENOTSUP:
#if defined(EOPNOTSUPP) && ENOTSUP != EOPNOTSUPP
    case EOPNOTSUPP:
#endif
        return ErrorKind::Unsupported;
    case ENOMEM:
        return ErrorKind::OutOfMemory;
    default:
        return ErrorKind::Other;
    }
}

Error Error::last_os_error() {
    return from_raw_os_error(errno);
}

}
}
}
