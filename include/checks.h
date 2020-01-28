#pragma once

#include "dttl.h"
#include "relations.h"
#include "state_machine.h"
#include <cstddef>

namespace simp::checks_internal {

template <class... Origins, class... Cells, size_t... I>
CONSTEXPR bool check_impl(const tape<Cells...> &tape, [[maybe_unused]] std::index_sequence<I...>) {
    using origins_types_list = utils::dttl<false, Origins...>;

    auto modifiers
        = utils::extract_unique(
            [](auto a) { return std::is_base_of_v<modifiers::modifier_base, decltype(a)>;}, tape);

    return (
    // TODO write type iterator
    state_machine::evaluate<utils::get_type<I, origins_types_list>>(modifiers, tape)
                                               && ... && true);
}

}
