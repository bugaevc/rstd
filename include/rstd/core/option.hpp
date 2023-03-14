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

    struct PrivateToken { };

    template<typename U>
    constexpr Option(PrivateToken, U &&value)
        : is_some_(true)
        , value(cxxstd::forward<U>(value))
    { }

public:
    static constexpr NoneType None = rstd::None;

    template<typename U>
    static constexpr Option Some(U &&value) {
        return Option { PrivateToken(), cxxstd::forward<U>(value) };
    }

    constexpr Option() noexcept
        : is_some_(false)
    { }

    constexpr Option(NoneType) noexcept
        : is_some_(false)
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
            value.construct((T &&) other.value.assume_init());
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
            value.construct((T &&) other.value.assume_init());
            other.value.destruct();
            other.is_some_ = false;
        }
        return *this;
    }

    constexpr bool is_some() const noexcept {
        return is_some_;
    }

    constexpr bool is_none() const noexcept {
        return !is_some_;
    }

    T &unwrap() & {
        if (!is_some_) {
            panic();
        }
        return value.assume_init();
    }

    const T &unwrap() const & {
        if (!is_some_) {
            panic();
        }
        return value.assume_init();
    }

    T &&unwrap() && {
        if (!is_some_) {
            panic();
        }
        return (T &&) value.assume_init();
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

template<typename T>
constexpr Option<T> Some(const T &value) {
    return Option<T>::Some(value);
}

template<typename T, typename = core::cxxstd::disable_if_lvalue_reference_t<T>>
constexpr Option<T> Some(T &&value) {
    return Option<T>::Some((T &&) value);
}
}
