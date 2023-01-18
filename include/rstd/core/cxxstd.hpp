#pragma once

// Placement new support.
template<typename T>
void *operator new(unsigned long, T *ptr) {
    return ptr;
}

namespace rstd {
namespace core {
namespace cxxstd {

template<typename T>
struct remove_reference { typedef T type; };
template<typename T>
struct remove_reference<T &> { typedef T type; };
template<typename T>
struct remove_reference<T &&> { typedef T type; };

template<typename T>
typename remove_reference<T>::type &&move(T &&obj) noexcept {
    return (typename remove_reference<T>::type &&) obj;
}

template<typename T>
T &&forward(typename remove_reference<T>::type &obj) noexcept {
    return (T &&) obj;
}

template<typename T>
T &&forward(typename remove_reference<T>::type &&obj) noexcept {
    return (T &&) obj;
}

template<typename T>
static constexpr bool templated_false = false;

template<typename T>
T &&declval() noexcept {
    static_assert(templated_false<T>, "declval() can not be actually evaluated");
}

template<typename...>
struct invoke_result { };

template<typename MethodDefBaseType, typename MethodType, typename InstanceType, typename... Args>
struct invoke_result<MethodType MethodDefBaseType::*, InstanceType, Args...> {
    using type = decltype(
        (
            forward<InstanceType>(declval<InstanceType>())
            .*declval<MethodType MethodDefBaseType::*>()
        )
        (forward<Args>(declval<Args>())...)
    );
};

template<typename F, typename... Args>
struct invoke_result<F, Args...> {
    using type = decltype(
        (forward<F>(declval<F>()))
        (forward<Args>(declval<Args>())...)
    );
};

}
}
}

