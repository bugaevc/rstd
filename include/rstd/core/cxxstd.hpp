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
using remove_reference_t = typename remove_reference<T>::type;

template<typename T>
struct is_lvalue_reference {
    constexpr static bool value = false;
};

template<typename T>
struct is_lvalue_reference<T &> {
    constexpr static bool value = true;
};

template<typename T>
remove_reference_t<T> &&move(T &&obj) noexcept {
    return (remove_reference_t<T> &&) obj;
}

template<typename T>
T &&forward(remove_reference_t<T> &obj) noexcept {
    return (T &&) obj;
}

template<typename T>
T &&forward(remove_reference_t<T> &&obj) noexcept {
    static_assert(!is_lvalue_reference<T>::value);
    return (T &&) obj;
}

template<typename T>
T &&declval() noexcept {
    // declval<T>() can never actually be evaluated; so cause
    // a compile-time error if its body is compiled.
    return (const T *) nullptr;
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

template<typename F, typename... Args>
using invoke_result_t = typename invoke_result<F, Args...>::type;

template<typename M, typename T>
class mem_fn_t {
    M T::*pm;

public:
    mem_fn_t(M T::*pm)
        : pm(pm)
    { }

    template<typename I, typename... Args>
    invoke_result_t<M T::*, I, Args...> operator()(I &&instance, Args &&...args) {
        return (forward<I>(instance).*pm)(forward<Args>(args)...);
    }
};

template<typename M, typename T>
mem_fn_t<M, T> mem_fn(M T::*pm) {
    return mem_fn_t<M, T>(pm);
}

template<bool B, typename T = void>
struct enable_if { };

template<typename T>
struct enable_if<true, T> {
    using type = T;
};

template<bool B, typename T = void>
using enable_if_t = typename enable_if<B, T>::type;

template<typename A, typename B>
struct is_same {
    constexpr static bool value = false;
};

template<typename T>
struct is_same<T, T> {
    constexpr static bool value = true;
};

template<typename A, typename B, typename T = void>
using enable_if_same_t = enable_if_t<is_same<A, B>::value, T>;

}
}
}
