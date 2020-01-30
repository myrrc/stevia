#pragma once

#include "../tl.h"
#include "relations.h"
#include "state_machine.h"
#include <cstddef>

#ifdef SIMP_LOG_FLOW
#include <iostream>
#endif

namespace simp::guard_internal {

using typelist::tl;
using typelist::utils::get;

template <class... Origins, class... Cells, size_t... I>
CONSTEXPR bool check_impl(const tl<true, Cells...> &tape, [[maybe_unused]] std::index_sequence<I...>) {
    tl<true, Origins...> origins;

    #ifdef SIMP_LOG_FLOW
    std::cout << "Checking origin types: " << type_name<tl<true, Origins...>>() << "\n"
              << "Cells tl: " << type_name<decltype(tape)>() << std::endl;
    #endif

    auto modifiers = typelist::utils::extract_unique_modifiers(tape);

    #ifdef SIMP_LOG_FLOW
    std::cout << "Found unique modifiers: " << type_name<decltype(modifiers)>() << std::endl;
    #endif

    return ((state_machine::evaluate<decltype(get<I>(origins))>(modifiers, tape)) && ... && true);
}

template <class... Origins, class... Cells> CONSTEXPR
bool where_impl(const tl<true, Origins...>&, const tl<true, Cells...> &tape) {
    return simp::guard_internal::check_impl<Origins...>(tape, std::index_sequence_for<Origins...>{});
}

template <bool cond>
struct assert_helper {
        CONSTEXPR assert_helper() {
            static_assert(cond, "Assertion failed");
        }
    };
}
