#pragma once

#include "core.h"

namespace stevia::internal {

constexpr const size_t precedence_min{1};
constexpr const size_t precedence_max{17};

// https://en.cppreference.com/w/cpp/language/operator_precedence
template<size_t Precedence, bool Unary> struct modifier_base {
    bool found{false};

    static constexpr const size_t precedence{Precedence};
    static constexpr const bool unary{Unary};

    CONSTEXPR void appeared() { found = true; }
    CONSTEXPR void reset() { found = false; }
};

template <class... Modifier> struct modifier {};

struct mod_or_base_t {};

template <> struct modifier<mod_or_base_t> : modifier_base<15, false> {
    [[nodiscard]] CONSTEXPR bool evaluate(bool state, bool relation_result) const {
        return state || relation_result;
    }
};

using mod_or = modifier<mod_or_base_t>;

struct mod_neg_base_t {};

template <> struct modifier<mod_neg_base_t> : modifier_base<3, true> {
    [[nodiscard]] CONSTEXPR bool evaluate(bool result) const {
        return !result;
    }
};

using mod_neg = modifier<mod_neg_base_t>;

} // namespace stevia::internal
