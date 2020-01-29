#pragma clang diagnostic push
#pragma ide diagnostic ignored "HidingNonVirtualFunction"
#pragma once

#include "core.h"
#include "modifiers.h"

#include <cstddef>
#include <functional>

namespace simp::typelist {

/**
 * Tuple-like container that can act either as a compile-time meta-type-list or a tuple.
 * Can store templated classes (template template parameters like foo<bar, baz>).
 */
template <bool pure, class...> struct tl { };

template <bool pure, class...>
struct tl_holder { };

template <class T>
struct tl_holder<true, T> {
    using type = T;

    CONSTEXPR const static bool pure = true;
    CONSTEXPR const static bool dependent = false;
};

template <class T>
struct tl_holder<false, T> {
    using type = T;

    CONSTEXPR const static bool pure = false;
    CONSTEXPR const static bool dependent = false;

    type value;
};

template <template <class...> class T, class ...Dependent>
struct tl_holder<true, T<Dependent...>> {
    using type = T<Dependent...>;
    using dependent_tl = tl<true, Dependent...>;

    CONSTEXPR const static bool pure = true;
    CONSTEXPR const static bool dependent = true;
};

template <template <class...> class T, class ...Dependent>
struct tl_holder<false, T<Dependent...>> {
    using type = T<Dependent...>;
    using dependent_tl = tl<true, Dependent...>;

    CONSTEXPR const static bool pure = false;
    CONSTEXPR const static bool dependent = true;

    type value;
};

/// Empty tl
template <bool pure> struct tl<pure> {};

/// One-element tl with a dependent type
template <bool pure, template <class...> class E, class... Dep>
struct tl<pure, E<Dep...>> : tl<pure>, tl_holder<pure, E<Dep...>> {
    using tail   = tl<pure>;
    using holder = tl_holder<pure, E<Dep...>>;

    CONSTEXPR tail& get_tail() { return *this; }
    CONSTEXPR const tail& get_tail() const { return *this; }

    CONSTEXPR holder& get_holder() { return *this; }
    CONSTEXPR const holder& get_holder() const { return *this; }
};

/// One-element tl with an independent type
template <bool pure, class E>
struct tl<pure, E> : tl<pure>, tl_holder<pure, E> {
    using tail   = tl<pure>;
    using holder = tl_holder<pure, E>;

    CONSTEXPR tail& get_tail() { return *this; }
    CONSTEXPR const tail& get_tail() const { return *this; }

    CONSTEXPR holder& get_holder() { return *this; }
    CONSTEXPR const holder& get_holder() const { return *this; }
};

/// Multi-element tl with head being dependent.
template <bool pure, template <class...> class E, class... Dep, class... Tail>
struct tl<pure, E<Dep...>, Tail...> : tl<pure, Tail...>, tl_holder<pure, E<Dep...>> {
    using tail   = tl<pure, Tail...>;
    using holder = tl_holder<pure, E<Dep...>>;

    CONSTEXPR tail& get_tail() { return *this; }
    CONSTEXPR const tail& get_tail() const { return *this; }

    CONSTEXPR holder& get_holder() { return *this; }
    CONSTEXPR const holder& get_holder() const { return *this; }
};

/// Multi-element tl with head being independent.
template <bool pure, class E,  class... Tail>
struct tl<pure, E, Tail...> : tl<pure, Tail...>, tl_holder<pure, E> {
    using tail   = tl<pure, Tail...>;
    using holder = tl_holder<pure, E>;

    CONSTEXPR tail& get_tail() { return *this; }
    CONSTEXPR const tail& get_tail() const { return *this; }

    CONSTEXPR holder& get_holder() { return *this; }
    CONSTEXPR const holder& get_holder() const { return *this; }
};

namespace utils {

template <size_t I, bool pure, class Type, class ...Types>
CONSTEXPR auto get(const tl<pure, Type, Types...>& list) {
    if CONSTEXPR_IF (I == 0) {
        return list.get_holder();
    } else {
        return get<I - 1>(list.get_tail());
    }
}

namespace {
template <class Functor, size_t... I, class... Args>
CONSTEXPR void apply_impl(Functor &&functor, tl<true, Args...> &list, std::index_sequence<I...>) {
    (functor(get<I>(list).value), ...);
}
}

template <class Functor, class... Args>
CONSTEXPR void apply(Functor &&functor, tl<true, Args...> &list) {
    apply_impl(std::forward<Functor>(functor), list, std::index_sequence_for<Args...>{});
}

template <class Target, class E>
CONSTEXPR auto find(const tl<true, E>& list) {
    if
    CONSTEXPR_IF(std::is_same_v<E, Target>) {
        return list.get_holder();
    } else {
        return tl_holder<false, Target>();
    }
}

namespace {
        template <class...> struct ne {};
        }

template <class Target, class First, class... Others,
    typename = std::enable_if_t<!std::is_same_v<ne<>, ne<Others...>>>
>
CONSTEXPR auto find(const tl<true, First, Others...> &list) {
    if CONSTEXPR_IF(std::is_same_v<First, Target>) {
        return list.get_holder();
    } else {
        return find<Target>(list.get_base());
    }
}

/// For guard

template <class Add, class E, class ...Tail>
CONSTEXPR auto add_front(const tl<true, E, Tail...>&) -> tl<true, Add, E, Tail...> {
    return {};
}

/// Single-element tl with a dependent head
template <template <class...> class E, class... Dep>
CONSTEXPR auto extract_unique(const tl<true, E<Dep...>> &)
    -> std::conditional_t<std::is_same_v<modifiers::relation_modifier<Dep...>, E<Dep...>>, tl<true, E<Dep...>>,
                          tl<true>> {
    return {};
}

/// Single-element tl with an in dependent head
template <class E, class... Dep>
CONSTEXPR auto extract_unique(const tl<true, E> &) -> tl<true> {
return {};
}


/// tl with a dependent head
template <template <class...> class E, class... Dep, class... Tail,
          typename = std::enable_if_t<!std::is_same_v<ne<>, ne<Tail...>>>>
CONSTEXPR auto extract_unique(const tl<true, E<Dep...>, Tail...> &list) {
    auto base_res = extract_unique(list.get_tail());
    using find_res = decltype(find<E<Dep...>>(list));

    if constexpr (
        !std::is_same_v<modifiers::relation_modifier<Dep...>, E<Dep...>> ||
        !std::is_same_v<find_res, tl_holder<false, E<Dep...>>>) {
            return base_res;
        }
    else {
        return add_front<E<Dep...>>(list);
    }
}

/// tl with an independent head
template <class E, class... Tail,
    typename = std::enable_if_t<!std::is_same_v<ne<>, ne<Tail...>>>>
CONSTEXPR auto extract_unique(const tl<true, E, Tail...> &list) {
    return extract_unique(list.get_tail());
}
} // namespace utils
}

#pragma clang diagnostic pop
