#pragma clang diagnostic push
#pragma ide diagnostic ignored "HidingNonVirtualFunction"
#pragma once

#include "core.h"
#include "modifiers.h"
#include "variadic_not_empty.h"

namespace stevia::internal {

/**
 * Tuple-like container that can act either as a compile-time meta-type-list or a tuple.
 * Stores templated classes only (template template parameters like foo<bar, baz>).
 */
template <bool pure, class...> struct tl { };

/**
 * Wrapper that is returned while accessing tl's element
 */
template <bool pure, class...>
struct tl_holder { };

/// Needed in state_machine::evaluate() only
template <class E>
struct tl_holder<true, E> {
    using type = E;
};

template <template <class...> class E, class ... Dep>
struct tl_holder<true, E<Dep...>> {
    using type = E<Dep...>;
};

template <template <class...> class T, class ...Dependent>
struct tl_holder<false, T<Dependent...>> {
    using type = T<Dependent...>;

    type value;
};

/// Empty tl
template <bool pure> struct tl<pure> {};

/// One-element tl
template <bool pure, template <class...> class E, class... Dep>
struct tl<pure, E<Dep...>> : tl<pure> {
    using tail_t   = tl<pure>;
    using holder_t = tl_holder<pure, E<Dep...>>;

    holder_t holder;

    CONSTEXPR tail_t get_tail() { return *this; }
    CONSTEXPR const tail_t get_tail() const { return *this; }

    CONSTEXPR holder_t& get_holder() { return holder; }
    CONSTEXPR const holder_t& get_holder() const { return holder; }
};

/// Multi-element tl.
template <bool pure, template <class...> class E, class... Dep, class... Tail>
struct tl<pure, E<Dep...>, Tail...> : tl<pure, Tail...> {
    using tail_t   = tl<pure, Tail...>;
    using holder_t = tl_holder<pure, E<Dep...>>;

    holder_t holder;

    CONSTEXPR tail_t get_tail() { return *this; }
    CONSTEXPR const tail_t get_tail() const { return *this; }

    CONSTEXPR holder_t& get_holder() { return holder; }
    CONSTEXPR const holder_t& get_holder() const { return holder; }
};

template <size_t I, bool pure, class E, class ... Tail>
CONSTEXPR auto get(const tl<pure, E, Tail...>& list) {
    if constexpr (I == 0) {
        return list.get_holder();
    } else {
        return get<I - 1>(list.get_tail());
    }
}

template <size_t I, class E, class ...Tail>
CONSTEXPR auto get() {
    if constexpr (I == 0) {
        return tl_holder<true, E>{};
    } else {
        return get<I - 1, Tail...>();
    }
}

namespace {
template <class Functor, size_t... I, class... E>
CONSTEXPR void apply_impl(Functor &&functor, tl<false, E...> &list, std::index_sequence<I...>) {
    (functor(get<I>(list).value), ...);
}
}

template <class Functor, class... E>
CONSTEXPR void apply(Functor &&functor, tl<false, E...> &list) {
    apply_impl(std::forward<Functor>(functor), list, std::index_sequence_for<E...>{});
}

template <class Target, bool pure>
CONSTEXPR auto find(const tl<pure>&) {
    return tl_holder<!pure /*to indicate that element was not found*/, Target>();
}

template <class Target, bool pure, class E>
CONSTEXPR auto find(const tl<pure, E>& list) {
    if constexpr (std::is_same_v<E, Target>) {
        return list.get_holder();
    } else {
        return tl_holder<!pure /*to indicate that element was not found*/, Target>();
    }
}

template <class Target, bool pure, class E, class... Tail, typename = enable_if_not_empty<Tail...>>
CONSTEXPR auto find(const tl<pure, E, Tail...> &list) {
    if constexpr (std::is_same_v<E, Target>) {
        return list.get_holder();
    } else {
        return find<Target>(list.get_tail());
    }
}

template <class Add>
CONSTEXPR auto add_front(const tl<true>&) -> tl<true, Add> {
    return {};
}

template <class Add, class E, class ...Tail>
CONSTEXPR auto add_front(const tl<true, E, Tail...>&) -> tl<true, Add, E, Tail...> {
    return {};
}
}

#pragma clang diagnostic pop
