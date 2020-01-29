#pragma once

#include "modifiers.h"
#include "tl.h"

namespace simp {

template <class... Args> using tape = typelist::tl<true, Args...>;

namespace relations {

struct relation_base {};

using modifiers::mod_neg;
using modifiers::mod_or;

template <template <class...> class Relation, class... Dependent> struct dependent_relation : relation_base {
    using type = Relation<Dependent...>;

    CONSTEXPR auto operator!() const -> tape<mod_neg, type> const { return {}; }

    template <template <class...> class OtherRel, class... OtherDep>
    CONSTEXPR auto operator&&(const OtherRel<OtherDep...> &) const -> tape<type, OtherRel<OtherDep...>> const {
        return {};
    }

    template <template <class...> class OtherRel, class... OtherDep>
    CONSTEXPR auto operator||(const OtherRel<OtherDep...> &) const -> tape<type, mod_or, OtherRel<OtherDep...>> const {
        return {};
    }

    template <class OtherRel> CONSTEXPR auto operator&&(const OtherRel &) const -> tape<type, OtherRel> const {
        return {};
    }

    template <class OtherRel> CONSTEXPR auto operator||(const OtherRel &) const -> tape<type, mod_or, OtherRel> const {
        return {};
    }
};

template <class Relation> struct relation : relation_base {
    using type = Relation;

    CONSTEXPR auto operator!() const { return tape<modifiers::mod_neg, type>{}; }

    template <template <class...> class OtherRel, class... OtherDep>
    CONSTEXPR auto operator&&(const OtherRel<OtherDep...> &) const -> tape<type, OtherRel<OtherDep...>> const {
        return {};
    }

    template <template <class...> class OtherRel, class... OtherDep>
    CONSTEXPR auto operator||(const OtherRel<OtherDep...> &) const
        -> tape<type, modifiers::mod_or, OtherRel<OtherDep...>> const {
        return {};
    }

    template <class OtherRel> CONSTEXPR auto operator&&(const OtherRel &) const -> tape<type, OtherRel> const {
        return {};
    }

    template <class OtherRel> CONSTEXPR auto operator||(const OtherRel &) const -> tape<type, mod_or, OtherRel> const {
        return {};
    }
};

} // namespace relations
} // namespace simp