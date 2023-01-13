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

public:
    ~Result() {
        if (is_ok_) {
            success.~T();
        } else {
            error.~E();
        }
    }

    constexpr bool is_ok() const {
        return is_ok_;
    }

    constexpr bool is_err() const {
        return !is_ok_;
    }

    T &unwrap() {
        if (!is_ok_) {
            panic();
        }
        return success;
    }

    const T &unwrap() const {
        if (!is_ok_) {
            panic();
        }
        return success;
    }

    E &unwrap_err() {
        if (is_ok_) {
            panic();
        }
        return error;
    }

    const E &unwrap_err() const {
        if (is_ok_) {
            panic();
        }
        return error;
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
}
