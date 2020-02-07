#pragma once

#include "relations.h"
#include "state_machine.h"
#include "tl.h"
#include <cstddef>

namespace stevia::internal {

template <template <class...> class E, class... Dep>
CONSTEXPR auto extract_unique_modifiers(const tl<true, E<Dep...>> &)
    -> std::conditional_t<std::is_same_v<modifier<Dep...>, E<Dep...>>, tl<true, E<Dep...>>, tl<true>> { return {}; }

template <template <class...> class E, class... Dep, class... Tail, typename = enable_if_not_empty<Tail...>>
CONSTEXPR auto extract_unique_modifiers(const tl<true, E<Dep...>, Tail...> &list) {
    auto tail_modifiers { extract_unique_modifiers(static_cast<const tl<true, Tail...>&>(list)) };
    auto tail_has_e     { find<E<Dep...>, true>(tail_modifiers) };

    constexpr bool need_to_add =
        std::is_same_v<modifier<Dep...>, E<Dep...> > &&
        std::is_same_v<decltype(tail_has_e), tl_holder<false, E<Dep...>>>;

    if constexpr (need_to_add) {
        return add_front<E<Dep...>>(tail_modifiers);
    } else {
        return tail_modifiers;
    }
}

template <class... Origins, class... Cells, size_t... I>
CONSTEXPR bool check_impl(const tl<true, Cells...> &tape, std::index_sequence<I...>) {
    STEVIA_LOG << "Checking origin types: " << type_name<tl<true, Origins...>>() << "\n"
               << "Cells tl: " << type_name<decltype(tape)>() << "\n";

    auto modifiers = extract_unique_modifiers(tape);

    STEVIA_LOG << "Found unique modifiers: " << type_name<decltype(modifiers)>() << "\n";

    return ((evaluate<decltype(get<I, Origins...>())>(modifiers, tape)) && ... && true);
}

template <class... Origins, class... Cells> CONSTEXPR
bool where_impl(const tl<true, Origins...>&, const tl<true, Cells...> &tape) {
    return check_impl<Origins...>(tape, std::index_sequence_for<Origins...>{});
}

template <bool cond>
struct assert_helper {
        CONSTEXPR assert_helper(std::string_view) {
            static_assert(cond, "Assertion failed");
        }
    };
}
