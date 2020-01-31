#pragma once

#include "relations.h"

using stevia::internal::tl;
using stevia::internal::mod_or;

template <template <class...> class Relation, class... Dep, class... OtherCells>
static CONSTEXPR auto operator&&(const Relation<Dep...> &, const tl<true, OtherCells...> &)
    -> tl<true, Relation<Dep...>, OtherCells...>{ return {}; }

template <template <class...> class Relation, class... Dep, class... OtherCells>
static CONSTEXPR auto operator||(const Relation<Dep...> &, const tl<true, OtherCells...> &)
    -> tl<true, Relation<Dep...>, mod_or, OtherCells...>{ return {}; }

template <template <class...> class Relation, class... Dep, class... OtherCells>
static CONSTEXPR auto operator&&(const tl<true, OtherCells...> &, const Relation<Dep...> &)
-> tl<true, OtherCells..., Relation<Dep...>>{ return {}; }

template <template <class...> class Relation, class... Dep, class... OtherCells>
static CONSTEXPR auto operator||(const tl<true, OtherCells...> &, const Relation<Dep...> &)
-> tl<true, OtherCells..., mod_or, Relation<Dep...>>{ return{}; }
