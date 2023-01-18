#pragma once

#include <rstd/core/cxxstd.hpp>
#include <rstd/core/panicking.hpp>

namespace rstd {
namespace core {
namespace result {

template<typename T, typename E>
class Result {
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

}
}

using core::result::Result;

template<typename T, typename E, typename U>
constexpr static Result<T, E> Ok(U &&value) {
    return Result<T, E>::Ok(core::cxxstd::forward<U>(value));
}

template<typename T, typename E, typename U>
constexpr static Result<T, E> Err(U &&err) {
    return Result<T, E>::Err(core::cxxstd::forward<U>(err));
}

}

