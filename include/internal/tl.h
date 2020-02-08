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

/**
 * Needed in state_machine::evaluate() only
 */
template <class E>
struct tl_holder<true, E> {
    using value_t = E;
};

template <template <class...> class E, class ... Dep>
struct tl_holder<true, E<Dep...>> {
    using value_t = E<Dep...>;
};

template <template <class...> class E, class ... Dep>
struct tl_holder<false, E<Dep...>> {
    using value_t = E<Dep...>;
    value_t  value;
};

template <bool pure, class T> CONSTEXPR static tl_holder<!pure, T> elem_not_found;

/// Empty tl
template <bool pure> struct tl<pure> {};

/// One-element tl
template <bool pure, template <class...> class E, class... Dep>
struct tl<pure, E<Dep...>> : tl<pure> {
    using holder_t = tl_holder<pure, E<Dep...>>;

    holder_t holder;

    CONSTEXPR holder_t& get_holder() { return holder; }
    CONSTEXPR const holder_t& get_holder() const { return holder; }
};

/// Multi-element tl.
template <bool pure, template <class...> class E, class... Dep, class... Tail>
struct tl<pure, E<Dep...>, Tail...> : tl<pure, Tail...> {
    using holder_t = tl_holder<pure, E<Dep...>>;

    holder_t holder;

    CONSTEXPR holder_t& get_holder() { return holder; }
    CONSTEXPR const holder_t& get_holder() const { return holder; }
};

template <size_t I, bool pure, class E, class ... Tail>
CONSTEXPR const auto& get(const tl<pure, E, Tail...>& list) {
    if constexpr (I == 0) {
        return list.get_holder();
    } else {
        return get<I - 1>(static_cast<const tl<pure, Tail...>&>(list));
    }
}

template <size_t I, bool pure, class E, class ... Tail>
CONSTEXPR auto& get(tl<pure, E, Tail...>& list) {
    if constexpr (I == 0) {
        return list.get_holder();
    } else {
        return get<I - 1>(static_cast<tl<pure, Tail...>&>(list));
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
CONSTEXPR void apply(Functor && functor, tl<false, E...> &list) {
    apply_impl(std::forward<Functor>(functor), list, std::index_sequence_for<E...>{});
}

template <class Target, bool pure>
CONSTEXPR const auto& find(const tl<pure>&) {
    return elem_not_found<pure, Target>;
}

template <class Target, bool pure>
CONSTEXPR auto& find(tl<pure>&) {
    return elem_not_found<pure, Target>;
}

template <class Target, bool pure, class E>
CONSTEXPR const auto& find(const tl<pure, E>& list) {
    if constexpr (std::is_same_v<E, Target>) {
        return list.get_holder();
    } else {
        return elem_not_found<pure, Target>;
    }
}

template <class Target, bool pure, class E>
CONSTEXPR auto& find(tl<pure, E>& list) {
    if constexpr (std::is_same_v<E, Target>) {
        return list.get_holder();
    } else {
        return elem_not_found<pure, Target>;
    }
}

template <class Target, bool pure, class E, class... Tail, typename = enable_if_not_empty<Tail...>>
CONSTEXPR const auto& find(const tl<pure, E, Tail...> &list) {
    if constexpr (std::is_same_v<E, Target>) {
        return list.get_holder();
    } else {
        return find<Target>(static_cast<const tl<pure, Tail...>&>(list));
    }
}

template <class Target, bool pure, class E, class... Tail, typename = enable_if_not_empty<Tail...>>
CONSTEXPR auto& find(tl<pure, E, Tail...> &list) {
    if constexpr (std::is_same_v<E, Target>) {
        return list.get_holder();
    } else {
        return find<Target>(static_cast<tl<pure, Tail...>&>(list));
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

template <bool pure, class One>
CONSTEXPR bool operator==(const tl_holder<pure, One> &, const tl_holder<pure, One> &) {
    return true;
}

template <bool pure, class One>
CONSTEXPR bool operator==(const tl_holder<pure, One> &,
                                        const tl_holder<!pure, One> &) {
    return false;
}

template <bool pure, class One, class Two>
CONSTEXPR bool operator==(const tl_holder<pure, One> &,
                                        const tl_holder<pure, Two> &) {
    return false;
}

template <bool pure, class ...One>
CONSTEXPR bool operator==(const tl<pure, One...> &, const tl<pure, One...> &) {
    return true;
}

template <bool pure, class ...One>
CONSTEXPR bool operator==(const tl<pure, One...> &, const tl<!pure, One...> &) {
    return false;
}

template <bool pure, class ...One, class ...Two>
CONSTEXPR static inline bool operator==(const tl<pure, One...> &,
                                        const tl<pure, Two...> &) {
    return false;
}

}