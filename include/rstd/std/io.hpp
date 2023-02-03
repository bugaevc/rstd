#pragma once

#include <rstd/core/slice.hpp>
#include <rstd/core/result.hpp>
#include <rstd/core/macros.hpp>
#include <rstd/core/tuple.hpp>
#include <rstd/core/cmp.hpp>
#include <rstd/alloc/vec.hpp>

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
    Error(ErrorKind kind)
        : kind_(kind)
    { }

    ErrorKind kind() const {
        return kind_;
    }

    static Error from_raw_os_error(i32 code);
    static Error last_os_error();
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
    Result<usize> read(core::slice::SliceMut<u8> buf);

    Result<UnitType> read_exact(core::slice::SliceMut<u8> buf) {
        while (!buf.is_empty()) {
            // TODO: Ignore ErrorKind::Interrupted
            usize nread = try(self().read(buf));
            if (nread == 0) {
                return Err(Error(ErrorKind::UnexpectedEof));
            }
            buf = buf[core::ops::RangeFrom<usize>(nread)];
        }

        return Ok(Unit);
    }
};

template<typename Self>
class Write {
private:
    Self &self() {
        return (Self &) *this;
    }

protected:
    Write() { }

public:
    Result<usize> write(core::slice::Slice<u8> buf);
    Result<UnitType> flush();

    Result<UnitType> write_all(core::slice::Slice<u8> buf) {
        while (!buf.is_empty()) {
            // TODO: Ignore ErrorKind::Interrupted
            usize nwritten = try(self().write(buf));
            if (nwritten == 0) {
                return Err(Error(ErrorKind::WriteZero));
            }
            buf = buf[core::ops::RangeFrom<usize>(nwritten)];
        }

        return Ok(Unit);
    }
};

template<typename Self>
class BufRead : public Read<Self> {
private:
    Self &self() {
        return (Self &) *this;
    }

protected:
    BufRead() { }

public:
    Result<core::slice::Slice<u8>> fill_buf();
    void consume(usize amount);

    Result<usize> read_until(u8 byte, Vec<u8> &buf) {
        usize total_consumed = 0;
        const u8 *p;
        do {
            core::slice::Slice<u8> ibuf = try(self().fill_buf());
            if (ibuf.is_empty()) {
                break;
            }
            p = (const u8 *) __builtin_memchr(ibuf.as_ptr(), byte, ibuf.len());
            usize to_consume;
            if (p) {
                to_consume = p - ibuf.as_ptr() + 1;
            } else {
                to_consume = ibuf.len();
            }
            buf.reserve(to_consume);
            __builtin_memcpy(buf.as_ptr() + buf.len(), ibuf.as_ptr(), to_consume);
            buf.set_len(buf.len() + to_consume);
            self().consume(to_consume);
            total_consumed += to_consume;
        } while (p == nullptr);
        return Ok(total_consumed);
    }

    // TODO: This should read into a String, not a Vec<u8>.
    Result<usize> read_line(Vec<u8> &buf) {
        return self().read_until('\n', buf);
    }
};

template<typename R>
class BufReader final : public BufRead<BufReader<R>> {
private:
    R inner;
    Vec<u8> buf;
    usize consumed { 0 };

public:
    BufReader(R &&inner)
        : inner((R &&) inner)
        , buf(Vec<u8>::with_capacity(8 * 1024))
    { }

    core::slice::Slice<u8> buffer() const {
        return buf;
    }

    usize capacity() const {
        return buf.capacity();
    }

    Result<core::slice::Slice<u8>> fill_buf() {
        using core::slice::Slice;
        using core::ops::RangeFrom;

        if (consumed < buf.len()) {
            Slice<u8> b = buf[RangeFrom<usize>(consumed)];
            return Ok(b);
        }
        consumed = 0;
        buf.set_len(buf.capacity());
        Result<usize> nread = inner.read(buf);
        if (nread.is_err()) {
            buf.set_len(0);
            return Err(nread.unwrap_err());
        }
        buf.set_len(nread.unwrap());
        return Ok((Slice<u8>) buf);
    }

    void consume(usize amount) {
        consumed += amount;
        if (consumed > buf.len()) {
            panic();
        }
    }
};

template<typename W>
class BufWriter : public Write<BufWriter<W>> {
private:
    W inner;
    Vec<u8> buf;

public:
    BufWriter(W &&inner)
        : inner((W &&) inner)
        , buf(Vec<u8>::with_capacity(8 * 1024))
    { }

    ~BufWriter() {
        (void) flush();
    }

    core::slice::Slice<u8> buffer() const {
        return buf;
    }

    usize capacity() const {
        return buf.capacity();
    }

    Result<usize> write(core::slice::Slice<u8> data) {
        usize available_space = buf.capacity() - buf.len();
        if (available_space == 0) {
            try(flush());
            available_space = buf.capacity();
        }
        usize to_copy = core::cmp::min(available_space, data.len());
        __builtin_memcpy(buf.as_ptr() + buf.len(), data.as_ptr(), to_copy);
        buf.set_len(buf.len() + to_copy);
        return Ok(to_copy);
    }

    Result<UnitType> flush() {
        using core::slice::Slice;
        using core::ops::RangeFrom;

        usize written_total = 0;
        while (written_total < buf.len()) {
            Slice<u8> to_write = buf[RangeFrom<usize>(written_total)];
            usize nwritten = try(inner.write(to_write));
            if (nwritten == 0) {
                return Err(Error(ErrorKind::WriteZero));
            }
            written_total += nwritten;
        }
        buf.clear();

        return inner.flush();
    }
};

class Stdout : public Write<Stdout> {
private:
    Stdout() { }
    friend Stdout stdout();

public:
    Result<usize> write(core::slice::Slice<u8> buf);
    Result<UnitType> flush();
};

Stdout stdout(void);

}
}
}
