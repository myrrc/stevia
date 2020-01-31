#pragma once

#include "modifiers.h"
#include "tl.h"

namespace stevia::internal {

struct relation_base {};

template <template <class...> class Relation, class... Dependent> struct relation : relation_base {
    using type = Relation<Dependent...>;

    CONSTEXPR auto operator!() const -> tl<true, mod_neg, type> const { return {}; }

    template <template <class...> class OtherRel, class... OtherDep>
    CONSTEXPR auto operator&&(const OtherRel<OtherDep...> &) const -> tl<true, type, OtherRel<OtherDep...>> const {
        return {};
    }

    template <template <class...> class OtherRel, class... OtherDep>
    CONSTEXPR auto operator||(const OtherRel<OtherDep...> &) const -> tl<true, type, mod_or, OtherRel<OtherDep...>> const {
        return {};
    }
};
} // namespace stevia