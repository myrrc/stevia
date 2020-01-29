#pragma once

#include "relations.h"
#include "state_machine.h"
#include "tl.h"
#include <cstddef>

namespace simp::checks_internal {

using typelist::tl;
using typelist::utils::extract_unique;
using typelist::utils::get;

template <class... Origins, class... Cells, size_t... I>
CONSTEXPR bool check_impl(const tl<true, Cells...> &tape, [[maybe_unused]] std::index_sequence<I...>) {
    tl<true, Origins...> origins;

    auto modifiers = extract_unique(tape);

    return ((state_machine::evaluate<decltype(get<I>(origins))>(modifiers, tape)) && ... && true);
}

}
