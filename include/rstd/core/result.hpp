#pragma once

#include <rstd/core/cxxstd.hpp>
#include <rstd/core/panicking.hpp>
#include <rstd/core/primitive.hpp>
#include <rstd/core/macros.hpp>
#include <rstd/core/mem/maybe-uninit.hpp>

namespace rstd {
namespace core {
namespace result {

template<typename T, typename E>
class must_use Result {
private:
    bool is_ok_;
    union {
        T success;
        E error;
    };

    Result() { }

public:
    ~Result() {
        if (is_ok_) {
            success.~T();
        } else {
            error.~E();
        }
    }

    Result(Result &&other)
        : is_ok_(other.is_ok_)
    {
        if (is_ok_) {
            new(&success) T(cxxstd::move(other.success));
        } else {
            new(&error) E(cxxstd::move(other.error));
        }
    }

    template<typename U>
    constexpr static Result Ok(U &&value) {
        Result res;
        res.is_ok_ = true;
        new(&res.success) T(cxxstd::forward<U>(value));
        return res;
    }

    template<typename U>
    constexpr static Result Err(U &&err) {
        Result res;
        res.is_ok_ = false;
        new(&res.error) E(cxxstd::forward<U>(err));
        return res;
    }

    constexpr bool is_ok() const {
        return is_ok_;
    }

    constexpr bool is_err() const {
        return !is_ok_;
    }

    T &unwrap() & {
        if (!is_ok_) {
            panic();
        }
        return success;
    }

    const T &unwrap() const & {
        if (!is_ok_) {
            panic();
        }
        return success;
    }

    T &&unwrap() && {
        if (!is_ok_) {
            panic();
        }
        return cxxstd::move(success);
    }

    E &unwrap_err() & {
        if (is_ok_) {
            panic();
        }
        return error;
    }

    const E &unwrap_err() const & {
        if (is_ok_) {
            panic();
        }
        return error;
    }

    E &&unwrap_err() && {
        if (is_ok_) {
            panic();
        }
        return cxxstd::move(error);
    }

    template<typename F>
    Result<typename cxxstd::invoke_result<F, T>::type, E> map(F f) {
        if (is_ok_) {
            return Ok(f(success));
        } else {
            return Err(error);
        }
    }

    template<typename F>
    Result<T, typename cxxstd::invoke_result<F, E>::type> map_err(F f) {
        if (is_ok_) {
            return Ok(success);
        } else {
            return Err(f(error));
        }
    }
};

template<typename T>
class must_use Result<T, Never> {
private:
    T success;

public:
    template<typename U>
    constexpr static Result Ok(U &&value) {
        mem::MaybeUninit<Result> res;
        new(res.as_bytes()) T(cxxstd::forward<U>(value));
        return cxxstd::move(res.assume_init());
    }

    constexpr bool is_ok() const {
        return true;
    }

    constexpr bool is_err() const {
        return false;
    }

    T &unwrap() & {
        return success;
    }

    const T &unwrap() const & {
        return success;
    }

    T &&unwrap() && {
        return (T &&) success;
    }

    Never unwrap_err() const {
        return panic();
    }

    template<typename F>
    Result<typename cxxstd::invoke_result<F, T>::type, Never> map(F f) {
        return Ok(f(success));
    }

    template<typename F>
    Result<T, Never> map_err(F f) {
        return *this;
    }

    // Support for implicitly converting Ok(val) from Result<T, Never> into
    // Result<T, E>.
    template<typename E>
    operator Result<T, E>() && {
        return Result<T, E>::Ok((T &&) success);
    }
};

template<typename E>
class must_use Result<Never, E> {
private:
    E error;

public:
    template<typename U>
    constexpr static Result Err(U &&err) {
        mem::MaybeUninit<Result> res;
        new(res.as_bytes()) E(cxxstd::forward<U>(err));
        return cxxstd::move(res.assume_init());
    }

    constexpr bool is_ok() const {
        return false;
    }

    constexpr bool is_err() const {
        return true;
    }

    Never unwrap() const {
        return panic();
    }

    E &unwrap_err() & {
        return error;
    }

    const E &unwrap_err() const & {
        return error;
    }

    E &&unwrap_err() && {
        return (E &&) error;
    }

    template<typename F>
    Result<Never, E> map(F f) {
        return *this;
    }

    template<typename F>
    Result<Never, typename cxxstd::invoke_result<F, E>::type> map_err(F f) {
        return Err(f(error));
    }

    // Support for implicitly converting Err(err) from Result<Never, E> into
    // Result<T, E>.
    template<typename T>
    operator Result<T, E>() && {
        return Result<T, E>::Err((E &&) error);
    }
};

}
}

using core::result::Result;

template<typename T>
constexpr static Result<T, core::Never> Ok(T value) {
    return Result<T, core::Never>::Ok((T &&) value);
}

template<typename E>
constexpr static Result<core::Never, E> Err(E err) {
    return Result<core::Never, E>::Err((E &&) err);
}

}
