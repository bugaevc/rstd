#pragma once

#include <rstd/core/option.hpp>
#include <rstd/core/cxxstd.hpp>

namespace rstd {
namespace core {
namespace iter {

template<typename I, typename F>
class Map;

template<typename I, typename P>
class Filter;

template<typename Self, typename Item>
class IntoIterator {
private:
    Self &self() {
        return (Self &) *this;
    }

protected:
    IntoIterator() { }

public:
    // Derived classes must implement this.
    // auto into_iter();

    // Implement the C++ iterator API to make iterators usable with for loops.
    class iterator {
    private:
        typedef typename cxxstd::invoke_result<decltype(&Self::into_iter), Self>::type MaybeOwnedIter;
        Option<MaybeOwnedIter> iter;
        Option<Item> item;

        friend class IntoIterator;
        iterator()
            : iter(None)
            , item(None)
        { }

        iterator(MaybeOwnedIter &&iter)
            : iter(Some<MaybeOwnedIter>(iter))
        {
            item = this->iter.unwrap().next();
        }

    public:
        Item &operator *() {
            return item.unwrap();
        }

        const Item& operator *() const {
            return item.unwrap();
        }

        iterator &operator ++() {
            item = iter.unwrap().next();
            return *this;
        }

        bool operator !=(const iterator &other) const {
            return item.is_some() != other.item.is_some();
        }
    };

    iterator begin() {
        return iterator(self().into_iter());
    }

    iterator end() {
        return iterator();
    }
};

template<typename Self, typename I>
class Iterator : public IntoIterator<Self, I> {
private:
    Self &self() {
        return (Self &) *this;
    }

protected:
    Iterator() { }
public:
    typedef I Item;

    // Derived classes must implement this.
    Option<Item> next();

    Self &into_iter() {
        return self();
    }

    template<typename F>
    void for_each(F f) {
        while (true) {
            Option<Item> item = self().next();
            if (item.is_none()) {
                break;
            }
            f(item.unwrap());
        }
    }

    template<typename F>
    Map<Self, F> map(F &&transform) {
        return Map<Self, F>(cxxstd::forward<Self>(self()), cxxstd::forward<F>(transform));
    }

    template<typename P>
    Filter<Self, P> filter(P &&predicate) {
        return Filter<Self, P>(cxxstd::forward<Self>(self()), cxxstd::forward<P>(predicate));
    }

    template<typename C>
    C collect() {
        return C::from_iter(cxxstd::forward<Self>(self()));
    }

    usize count() {
        usize c = 0;
        for (Item &item : self()) {
            c++;
        }
        return c;
    }

    template<typename P>
    bool all(P predicate) {
        for (Item &item : self()) {
            if (!predicate(item)) {
                return false;
            }
        }
        return true;
    }

    template<typename P>
    bool any(P predicate) {
        for (Item &item : self()) {
            if (predicate(item)) {
                return true;
            }
        }
        return false;
    }
};


template<typename I, typename F>
class Map : public Iterator<Map<I, F>, typename cxxstd::invoke_result<F, typename I::Item>::type> {
private:
    I inner;
    F transform;

public:
    Map(I &&inner, F &&transform)
        : inner(cxxstd::forward<I>(inner))
        , transform(cxxstd::forward<F>(transform))
    { }

    Option<typename cxxstd::invoke_result<F, typename I::Item>::type> next() {
        Option<typename I::Item> item = inner.next();
        if (item.is_none()) {
            return None;
        }
        return Some(transform(item.unwrap()));
    }
};

template<typename I, typename P>
class Filter : public Iterator<Filter<I, P>, typename I::Item> {
private:
    I inner;
    P predicate;

public:
    Filter(I &&inner, P &&predicate)
        : inner(cxxstd::forward<I>(inner))
        , predicate(cxxstd::forward<P>(predicate))
    { }

    Option<typename I::Item> next() {
        while (true) {
            Option<typename I::Item> item = inner.next();
            if (item.is_none()) {
                return None;
            }
            if (predicate(item.unwrap())) {
                return item;
            }
        }
    }
};

template<typename T>
class Empty : public Iterator<Empty<T>, T> {
public:
    Option<T> next() {
        return None;
    }
};

template<typename T>
Empty<T> empty() {
    return Empty<T>();
}

template<typename F, typename T>
class FromFn : public Iterator<FromFn<F, T>, T> {
private:
    F fn;

public:
    FromFn(F fn)
        : fn(fn)
    { }

    Option<T> next() {
        return fn();
    }
};

template<typename F, typename T>
FromFn<F, T> from_fn(F fn) {
    return FromFn<F, T>(fn);
}

template<typename T>
class Once : public Iterator<Once<T>, T> {
private:
    Option<T> item;

public:
    Once(T &&item)
        : item(cxxstd::forward<T>(item))
    { }

    Option<T> next() {
        return item.take();
    }
};

template<typename T>
Once<T> once(T &&item) {
    return Once<T>(cxxstd::forward<T>(item));
}

template<typename T>
class Repeat : public Iterator<Repeat<T>, T> {
private:
    T value;

public:
    Repeat(T &&value)
        : value(cxxstd::forward<T>(value))
    { }

    Option<T> next() {
        return Some(value);
    }
};

template<typename T>
Repeat<T> repeat(T &&value) {
    return Repeat<T>(cxxstd::forward<T>(value));
}

}
}
}
