#pragma once

#include "dttl.h"
#include "relations.h"
#include "state_machine.h"
#include <cstddef>

namespace simp::checks_internal {

struct functor_t {
    constexpr std::true_type operator()(modifiers::modifier_base&)  { return {};};

    template <class T>
    constexpr std::false_type operator()(T&) { return {}; }
};

constexpr static const functor_t functor;

template <class... Origins, class... Cells, size_t... I>
CONSTEXPR bool check_impl(const tape<Cells...> &tape, [[maybe_unused]] std::index_sequence<I...>) {
    using origins_types_list = utils::dttl<false, Origins...>;

    auto modifiers = utils::extract_unique(functor, tape);

    return (
    // TODO write type iterator
    state_machine::evaluate<utils::get_type<I, origins_types_list>>(modifiers, tape)
                                               && ... && true);
}

}
