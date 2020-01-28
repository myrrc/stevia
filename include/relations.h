#pragma once

#include "modifiers.h"

namespace simp {

template <class... Args> using tape = utils::dttl<false, Args...>;

namespace relations {

struct relation_base {};

template <template <class...> class Relation, class... Dependent> struct dependent_relation : relation_base {
    using type = Relation<Dependent...>;

    CONSTEXPR auto operator!() const { return tape<modifiers::mod_neg, type>{}; }

    template <template <class...> class OtherRel, class... OtherDep,
              typename = std::enable_if_t<std::is_base_of_v<relation_base, OtherRel>>>
    CONSTEXPR auto operator&&(const OtherRel<OtherDep...> &) const {
        return tape<type, OtherRel<OtherDep>...>{};
    }

    template <template <class...> class OtherRel, class... OtherDep,
              typename = std::enable_if_t<std::is_base_of_v<relation_base, OtherRel>>>
    CONSTEXPR auto operator||(const OtherRel<OtherDep...> &) const {
        return tape<type, modifiers::mod_or, OtherRel<OtherDep...>>{};
    }

    template <class OtherRel, typename = std::enable_if_t<std::is_base_of_v<relation_base, OtherRel>>>
    CONSTEXPR auto operator&&(const OtherRel &other) const {
        return tape<type, OtherRel>{};
    }

    template <class OtherRel, typename = std::enable_if_t<std::is_base_of_v<relation_base, OtherRel>>>
    CONSTEXPR auto operator||(const OtherRel &other) const {
        return tape<type, OtherRel>{};
    }
};

template <class Relation> struct relation : relation_base {
    using type = Relation;

    CONSTEXPR auto operator!() const { return tape<modifiers::mod_neg, type>{}; }

    template <template <class...> class OtherRel, class... OtherDep,
              typename = std::enable_if_t<std::is_base_of_v<relation_base, OtherRel>>>
    CONSTEXPR auto operator&&(const OtherRel<OtherDep...> &) const {
        return tape<type, OtherRel<OtherDep>...>{};
    }

    template <template <class...> class OtherRel, class... OtherDep,
              typename = std::enable_if_t<std::is_base_of_v<relation_base, OtherRel>>>
    CONSTEXPR auto operator||(const OtherRel<OtherDep...> &) const {
        return tape<type, modifiers::mod_or, OtherRel<OtherDep...>>{};
    }

    template <class OtherRel, typename = std::enable_if_t<std::is_base_of_v<relation_base, OtherRel>>>
    CONSTEXPR auto operator&&(const OtherRel &other) const {
        return tape<type, OtherRel>{};
    }

    template <class OtherRel, typename = std::enable_if_t<std::is_base_of_v<relation_base, OtherRel>>>
    CONSTEXPR auto operator||(const OtherRel &other) const {
        return tape<type, OtherRel>{};
    }
};

} // namespace relations
} // namespace simp