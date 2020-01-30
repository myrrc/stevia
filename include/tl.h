#pragma clang diagnostic push
#pragma ide diagnostic ignored "HidingNonVirtualFunction"
#pragma once

#include "internal/core.h"
#include "internal/modifiers.h"
#include "../contrib/variadic_not_empty.h"

#include <cstddef>
#include <functional>

namespace simp::typelist {

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

template <template <class...> class E, class ... Dep>
struct tl_holder<true, E<Dep...>> {
    using type = E<Dep...>;
    using dependent_tl = tl<true, Dep...>;
};

template <template <class...> class T, class ...Dependent>
struct tl_holder<false, T<Dependent...>> {
    using type = T<Dependent...>;
    using dependent_tl = tl<true, Dependent...>;

    type value;
};

/// Empty tl
template <bool pure> struct tl<pure> {};

/// One-element tl
template <bool pure, template <class...> class E, class... Dep>
struct tl<pure, E<Dep...>> : tl<pure>, tl_holder<pure, E<Dep...>> {
    using tail   = tl<pure>;
    using holder = tl_holder<pure, E<Dep...>>;

    CONSTEXPR tail& get_tail() { return *this; }
    CONSTEXPR const tail& get_tail() const { return *this; }

    CONSTEXPR holder& get_holder() { return *this; }
    CONSTEXPR const holder& get_holder() const { return *this; }
};

/// Multi-element tl.
template <bool pure, template <class...> class E, class... Dep, class... Tail>
struct tl<pure, E<Dep...>, Tail...> : tl<pure, Tail...>, tl_holder<pure, E<Dep...>> {
    using tail   = tl<pure, Tail...>;
    using holder = tl_holder<pure, E<Dep...>>;

    CONSTEXPR tail& get_tail() { return *this; }
    CONSTEXPR const tail& get_tail() const { return *this; }

    CONSTEXPR holder& get_holder() { return *this; }
    CONSTEXPR const holder& get_holder() const { return *this; }
};

namespace utils {

template <size_t I, bool pure, class E, class ... Tail>
CONSTEXPR auto get(const tl<pure, E, Tail...>& list) {
    if constexpr (I == 0) {
        return list.get_holder();
    } else {
        return get<I - 1>(list.get_tail());
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

/// For guard

template <class Add, class E, class ...Tail>
CONSTEXPR auto add_front(const tl<true, E, Tail...>&) -> tl<true, Add, E, Tail...> {
    return {};
}

template <template <class...> class E, class... Dep>
CONSTEXPR auto extract_unique_modifiers(const tl<true, E<Dep...>> &)
    -> std::conditional_t<std::is_same_v<modifiers::relation_modifier<Dep...>, E<Dep...>>, tl<true, E<Dep...>>,
                          tl<true>> {
    return {};
}

template <template <class...> class E, class... Dep, class... Tail, typename = enable_if_not_empty<Tail...>>
CONSTEXPR auto extract_unique_modifiers(const tl<true, E<Dep...>, Tail...> &list) {
    return
        /// Current type is a modifier
        (std::is_same_v<modifiers::relation_modifier<Dep...>, E<Dep...>> &&
         /// Tail does not contain it
         std::is_same_v<decltype(find<true, E<Dep...>>(list.get_tail())), tl_holder<false, E<Dep...>>>)
            ? add_front<E<Dep...>>(list)
            :                                          /// add it
            extract_unique_modifiers(list.get_tail()); /// else return the tail
}
} // namespace utils
}

#pragma clang diagnostic pop
