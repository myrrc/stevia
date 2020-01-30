#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#pragma once

#include "core.h"

namespace simp::modifiers {

struct modifier_base {};
template <class ...Modifier> struct relation_modifier : modifier_base {
    bool found{false};

    CONSTEXPR void appeared() { found = true; }
    CONSTEXPR void reset() { found = false; }

    [[nodiscard]] CONSTEXPR bool evaluate(bool state, bool relation_result) const;
};

struct mod_or_base_t {};

template <> struct relation_modifier<mod_or_base_t> : modifier_base {
    bool found{false};

    CONSTEXPR void appeared() { found = true; }
    CONSTEXPR void reset() { found = false; }

    [[nodiscard]] CONSTEXPR bool evaluate(bool state, bool relation_result) const {
        return found ? state || relation_result : state && relation_result;
    }
};

using mod_or = relation_modifier<mod_or_base_t>;

struct mod_neg_base_t {};

template <> struct relation_modifier<mod_neg_base_t> : modifier_base {
    bool found{false};

    CONSTEXPR void appeared() { found = true; }
    CONSTEXPR void reset() { found = false; }

    [[nodiscard]] CONSTEXPR bool evaluate(bool state, bool relation_result) const {
        return state && found == !relation_result;
    }
};

using mod_neg = relation_modifier<mod_neg_base_t>;

} // namespace simp::modifiers

#pragma clang diagnostic pop