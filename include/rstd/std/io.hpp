#pragma once

#include <rstd/core/slice.hpp>
#include <rstd/core/result.hpp>
#include <rstd/core/macros.hpp>

namespace rstd {
namespace std {
namespace io {

enum class ErrorKind {
    NotFound,
    PermissionDenied,
    ConnectionRefused,
    ConnectionReset,
    HostUnreachable,
    NetworkUnreachable,
    ConnectionAborted,
    NotConnected,
    AddrInUse,
    AddrNotAvailable,
    NetworkDown,
    BrokenPipe,
    AlreadyExists,
    WouldBlock,
    NotADirectory,
    IsADirectory,
    DirectoryNotEmpty,
    ReadOnlyFilesystem,
    FilesystemLoop,
    StaleNetworkFileHandle,
    InvalidInput,
    InvalidData,
    TimedOut,
    WriteZero,
    StorageFull,
    NotSeekable,
    FilesystemQuotaExceeded,
    FileTooLarge,
    ResourceBusy,
    ExecutableFileBusy,
    Deadlock,
    CrossesDevices,
    TooManyLinks,
    InvalidFilename,
    ArgumentListTooLong,
    Interrupted,
    Unsupported,
    UnexpectedEof,
    OutOfMemory,
    Other,
};

class Error {
private:
    ErrorKind kind_;

public:
    ErrorKind kind() const {
        return kind_;
    }
};

template<typename T>
using Result = core::result::Result<T, Error>;

template<typename Self>
class Read {
private:
    Self &self() {
        return (Self &) *this;
    }

protected:
    Read() { }

public:
    Result<usize> read(SliceMut<u8> buf);

    Result<void> read_exact(SliceMut<u8> buf) {
        while (!buf.is_empty()) {
            // TODO: Ignore ErrorKind::Interrupted
            usize nread = try(self().read(buf));
            if (nread == 0) {
                return Err(Error(ErrorKind::UnexpectedEof));
            }
            buf = buf[core::ops::RangeFrom(nread)];
        }

        Ok()
    }
};

}
}
}
