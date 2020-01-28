#pragma once

#include "relations.h"

template <template <class...> class Relation, class... Dep, class... OtherCells>
static constexpr auto operator&&(const Relation<Dep...> &left, const simp::tape<OtherCells...> &right) {
    return simp::tape<Relation<Dep...>, OtherCells...>{};
}

template <template <class...> class Relation, class... Dep, class... OtherCells>
static constexpr auto operator||(const Relation<Dep...> &left, const simp::tape<OtherCells...> &right) {
    return simp::tape<Relation<Dep...>, simp::modifiers::mod_or, OtherCells...>{};
}

template <template <class...> class Relation, class... Dep, class... OtherCells>
static constexpr auto operator&&(const simp::tape<OtherCells...> &left, const Relation<Dep...> &right) {
    return simp::tape<OtherCells..., Relation<Dep...>>{};
}

template <template <class...> class Relation, class... Dep, class... OtherCells>
static constexpr auto operator||(const simp::tape<OtherCells...> &left, const Relation<Dep...> &right) {
    return simp::tape<OtherCells..., simp::modifiers::mod_or, Relation<Dep...>>{};
}