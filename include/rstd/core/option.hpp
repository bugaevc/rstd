#pragma once

#include <rstd/core/mem/maybe-uninit.hpp>
#include <rstd/core/cxxstd.hpp>
#include <rstd/core/panicking.hpp>

namespace rstd {

static constexpr class NoneType { } None { };

namespace core {
namespace option {

template<typename T>
class Option {
private:
    bool is_some_;
    mem::MaybeUninit<T> value;

public:
    static constexpr typeof(rstd::None) None = rstd::None;
    static Option Some(T &&value) {
        return Option(cxxstd::forward<T>(value));
    }

    constexpr Option() noexcept
        : is_some_(false)
    { }

    constexpr Option(typeof(None)) noexcept
        : is_some_(false)
    { }

    Option(T &&value)
        : is_some_(true)
        , value(cxxstd::forward<T>(value))
    { }

    Option(Option &other)
        : is_some_(other.is_some_)
    {
        if (is_some_) {
            value.construct(other.value.assume_init());
        }
    }

    Option(Option &&other)
        : is_some_(other.is_some_)
    {
        if (is_some_) {
            value.construct(cxxstd::forward<T>(other.value.assume_init()));
            other.value.destruct();
            other.is_some_ = false;
        }
    }

    ~Option() {
        if (is_some_) {
            value.destruct();
        }
    }

    Option& operator =(Option &other) {
        if (is_some_) {
            value.destruct();
        }
        is_some_ = other.is_some_;
        if (is_some_) {
            value.construct(other.value.assume_init());
        }
        return *this;
    }

    Option& operator =(Option &&other) {
        if (is_some_) {
            value.destruct();
        }
        is_some_ = other.is_some_;
        if (is_some_) {
            value.construct(cxxstd::move(other.value.assume_init()));
            other.value.destruct();
            other.is_some_ = false;
        }
        return *this;
    }

    bool is_some() const {
        return is_some_;
    }

    bool is_none() const {
        return !is_some_;
    }

    T &unwrap() {
        if (!is_some_) {
            panic();
        }
        return value.assume_init();
    }

    const T &unwrap() const {
        if (!is_some_) {
            panic();
        }
        return value.assume_init();
    }

    bool operator ==(const Option &other) const {
        if (is_some_ != other.is_some_) {
            return false;
        }
        if (!is_some_) {
            return true;
        }
        return value.assume_init() == other.value.assume_init();
    }

    Option<T> take() {
        return Option(cxxstd::move(*this));
    }
};

}
}

using core::option::Option;

template <typename T>
Option<T> Some(T &&value) {
    return Option<T>(core::cxxstd::forward<T>(value));
}
}
