#pragma once

#include "relations.h"
#include "state_machine.h"
#include "tl.h"

namespace stevia::internal {

template <template <class...> class E, class... Dep>
CONSTEXPR auto extract_unique_modifiers(const tl<true, E<Dep...>> &)
    -> std::conditional_t<std::is_same_v<modifier<Dep...>, E<Dep...>>, tl<true, E<Dep...>>, tl<true>> { return {}; }

template <template <class...> class E, class... Dep, class... Tail, typename = enable_if_not_empty<Tail...>>
CONSTEXPR auto extract_unique_modifiers(const tl<true, E<Dep...>, Tail...> &list) {
    return
        /// Current type is a modifier
        (std::is_same_v<modifier<Dep...>, E<Dep...> > &&
        /// Tail does not contain it
         std::is_same_v<decltype(find<E<Dep...>>(list.get_tail())), tl_holder<false, E<Dep...>>>)
         ? add_front<E<Dep...>>(list)                 /// add it
         : extract_unique_modifiers(list.get_tail()); /// else return the tail
}

template <class... Origins, class... Cells, size_t... I>
CONSTEXPR bool check_impl(const tl<true, Cells...> &tape, std::index_sequence<I...>) {
    tl<true, Origins...> origins;

    STEVIA_LOG << "Checking origin types: " << type_name<tl<true, Origins...>>() << "\n"
             << "Cells tl: " << type_name<decltype(tape)>() << std::endl;

    auto modifiers = extract_unique_modifiers(tape);

    STEVIA_LOG << "Found unique modifiers: " << type_name<decltype(modifiers)>() << std::endl;

    return ((evaluate<decltype(get<I>(origins))>(modifiers, tape)) && ... && true);
}

template <class... Origins, class... Cells> CONSTEXPR
bool where_impl(const tl<true, Origins...>&, const tl<true, Cells...> &tape) {
    return check_impl<Origins...>(tape, std::index_sequence_for<Origins...>{});
}

template <bool cond>
struct assert_helper {
        CONSTEXPR assert_helper() {
            static_assert(cond, "Assertion failed");
        }
    };
}
