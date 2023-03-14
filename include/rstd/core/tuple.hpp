#pragma once

#include <rstd/core/cxxstd.hpp>
#include <rstd/core/primitive.hpp>

namespace rstd {
namespace core {
namespace tuple {

template<typename... Ts>
class Tuple;

namespace __internal {

template<usize index, typename T, typename... Ts>
struct getter {
    typedef typename getter<index - 1, Ts...>::type type;

    static const type &get(const Tuple<T, Ts...> &tuple) {
        return getter<index - 1, Ts...>::get(tuple.tail);
    }

    static type &get(Tuple<T, Ts...> &tuple) {
        return getter<index - 1, Ts...>::get(tuple.tail);
    }
};

template<typename T, typename... Ts>
struct getter<0, T, Ts...> {
    typedef T type;

    static const T &get(const Tuple<T, Ts...> &tuple) {
        return tuple.head;
    }

    static T &get(Tuple<T, Ts...> &tuple) {
        return tuple.head;
    }
};

template<typename... Ts>
class Tie;

template<>
class Tie<> {
public:
    Tie<> &operator = (const Tuple<> &) {
        return *this;
    }
    Tie<> &operator = (Tuple<> &&) {
        return *this;
    }
};

template<typename T, typename... Ts>
class Tie<T, Ts...> {
private:
    T &head;
    Tie<Ts...> tail;

public:
    Tie(T &head, Tie<Ts...> tail)
        : head(head)
        , tail(tail)
    { }

    Tie<T, Ts...> &operator = (const Tuple<T, Ts...> &tuple) {
        head = tuple.head;
        tail = tuple.tail;
        return *this;
    }

    Tie<T, Ts...> &operator = (Tuple<T, Ts...> &&tuple) {
        head = (T &&) tuple.head;
        tail = (Tuple<Ts...> &&) tuple.tail;
        return *this;
    }
};

}

template<>
class Tuple<> { };

template<typename T, typename... Ts>
class Tuple<T, Ts...> {
private:
    T head;
    Tuple<Ts...> tail;

    template<usize index>
    friend struct __internal::getter;
    friend class __internal::Tie<T, Ts...>;

public:
    template<typename U, typename... Us>
    explicit Tuple(U &&head, Us &&...tail)
        : head(cxxstd::forward<U>(head))
        , tail(cxxstd::forward<Us>(tail)...)
    { }

    template<usize index>
    using element_type = typename __internal::getter<index, T, Ts...>::type;

    template<usize index>
    const element_type<index> &get() const {
        return __internal::getter<index, T, Ts...>::get(*this);
    }

    template<usize index>
    element_type<index> &get() {
        return __internal::getter<index, T, Ts...>::get(*this);
    }
};

typedef Tuple<> UnitType;
static constexpr UnitType Unit { };

template<typename... Ts>
static inline __internal::Tie<Ts...> tie(Ts &...);

template<>
inline  __internal::Tie<> tie() {
    return __internal::Tie<>();
}

template<typename T, typename... Ts>
inline  __internal::Tie<T, Ts...> tie(T &first, Ts &...rest) {
    return __internal::Tie<T, Ts...> { first, tie<Ts...>(rest...) };
}

}
}

using core::tuple::Tuple;
using core::tuple::Unit;
using core::tuple::UnitType;
}
