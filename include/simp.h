#include "type_traits"
#include <tuple>
#include <utility>

namespace simp {

namespace modifiers {

template <class Modifier> struct modifier_t {};

struct mod_or_t {};
struct mod_neg_t {};

constexpr static const mod_or_t mod_or;
constexpr static const mod_neg_t mod_neg;

} // namespace modifiers

/**
 * Turing-like machine tape.
 * Tape consists of \c Cells.
 * Each \c Cell is either a relation modifier (binary or unary)
 * or a relation (dependent or independent).
 */
template <class... Cells> struct tape {
  constexpr explicit tape(const Cells &... cells_) : cells(cells_...) {}
  constexpr explicit tape(std::tuple<Cells...> other) : cells(std::move(other)) {}

  template <class... OtherCells>
  constexpr auto operator&&(const tape<OtherCells...> &other) const {
    return tape<Cells..., OtherCells...>(std::tuple_cat(cells, other.cells));
  }

  template <class... OtherCells>
  constexpr auto operator||(const tape<OtherCells...> &other) const {
    return tape<Cells..., modifiers::mod_or_t, OtherCells...>(
        std::tuple_cat(cells, std::make_tuple(modifiers::mod_or), other.cells));
  }

  std::tuple<Cells...> cells;
};

namespace relations {
struct relation_base {};
} // namespace relations

template <class Origin> struct state_machine {
  bool evaluated = true;

  bool use_or = false;
  bool negate = false;

  template <class... Args>
  constexpr explicit state_machine(const tape<Args...> &final_condition) {
    parse(final_condition.cells, std::index_sequence_for<Args...>{});
  }

  template <class... Args, size_t... I>
  constexpr void parse(const std::tuple<Args...> &args,
                       std::index_sequence<I...> seq) {
    ((void)parse_item(std::get<I>(args)), ...);
  }

  template <class Item> constexpr void parse_item(const Item &item) {
    if constexpr (std::is_same_v<Item, modifiers::mod_or_t>) {
      use_or = false;
      return;
    }

    if constexpr (std::is_same_v<Item, modifiers::mod_neg_t>) {
      negate = true;
      return;
    }

    if constexpr (std::is_base_of_v<relations::relation_base, Item>) {
      if (use_or || evaluated) {
        bool rel_res = parse_relation(item);

        if (use_or) {
          evaluated |= negate == !rel_res;
        } else {
          evaluated &= negate == !rel_res;
        }
      }

      negate = false;
      use_or = false;
    }
  }

  template <template <class...> class DepRel, class... Dependent>
  constexpr bool parse_relation(const DepRel<Dependent...> &rel) const {
    return rel.template check<Origin>;
    // return storage.find<Origin, Dependent...>(rel);
  }

  template <class Relation>
  constexpr bool parse_relation(const Relation &rel) const {
    return rel.template check<Origin>;
    // return storage.find<Origin, Dependent...>(rel);
  }
};

namespace relations {

template <template <class...> class Relation, class... Dependent>
struct dependent_relation : relation_base {
  [[nodiscard]] constexpr auto make() const { return Relation<Dependent...>{}; }

  constexpr auto operator!() const {
    return tape(modifiers::mod_neg, make());
  }

  template <template <class...> class OtherDepRel, class... OtherDep>
  constexpr auto operator&&(const OtherDepRel<OtherDep...> &other) const {
    return tape(make(), other);
  }

  template <template <class...> class OtherDepRel, class... OtherDep>
  constexpr auto operator||(const OtherDepRel<OtherDep...> &other) const {
    return tape(make(), modifiers::mod_or, other.make());
  }

  template <class OtherRel>
  constexpr auto operator&&(const OtherRel &other) const {
    return tape(make(), other);
  }

  template <class OtherRel>
  constexpr auto operator||(const OtherRel &other) const {
    return tape(make(), modifiers::mod_or, other.make());
  }
};

template <class Relation> struct relation : relation_base {
  [[nodiscard]] constexpr auto make() const { return Relation{}; }

  constexpr auto operator!() const {
    return tape(modifiers::mod_neg, make());
  }

  template <template <class...> class OtherDepRel, class... OtherDep>
  constexpr auto operator&&(const OtherDepRel<OtherDep...> &other) const {
    return tape(make(), other);
  }

  template <template <class...> class OtherDepRel, class... OtherDep>
  constexpr auto operator||(const OtherDepRel<OtherDep...> &other) const {
    return tape(make(), modifiers::mod_or, other.make());
  }

  template <class OtherRel>
  constexpr auto operator&&(const OtherRel &other) const {
    return tape(make(), other);
  }

  template <class OtherRel>
  constexpr auto operator||(const OtherRel &other) const {
    return tape(make(), modifiers::mod_or, other.make());
  }
};

} // namespace relations

namespace operators {

template <template <class...> class Relation, class... A, class... B>
static constexpr auto operator&&(const Relation<A...> &left,
                                 const tape<B...> &right) {
  return tape(left) && right;
}

template <template <class...> class Relation, class... A, class... B>
static constexpr auto operator||(const Relation<A...> &left,
                                 const tape<B...> &right) {
  return tape(left) || right;
}

template <template <class...> class Relation, class... A, class... B>
static constexpr auto operator&&(const tape<B...> &left,
                                 const Relation<A...> &right) {
  return left && tape(right);
}

template <template <class...> class Relation, class... A, class... B>
static constexpr auto operator||(const tape<B...> &left,
                                 const Relation<A...> &right) {
  return left && tape(right);
}

template <class... Origins, class... Args, size_t... I>
constexpr bool check_impl(const tape<Args...> &cond,
                          std::index_sequence<I...> _) {
  using tuple_t = std::tuple<Origins...>;
  return (state_machine<std::tuple_element_t<I, tuple_t>>(cond).evaluated && ... &&
          true);
}
} // namespace operators

namespace checks {
template <class... Origins, class... Args>
constexpr bool check(const tape<Args...> &cond) {
  return operators::check_impl<Origins...>(
      cond, std::index_sequence_for<Origins...>{});
}

template <class... Origins, template <class...> class DepRel,
          class... Dependent>
constexpr bool check(const DepRel<Dependent...> &rel) {
  return check<Origins...>(tape(rel.make()));
}

template <class... Origins, class Relation>
constexpr bool check(const Relation &rel) {
  return check<Origins...>(tape(rel.make()));
}
} // namespace checks

using namespace operators;
using namespace checks;

#define simp_relation(__name, ___dep, __cdep, __cond)                          \
  template <___dep>                                                            \
  struct __name##_rel_t                                                        \
      : simp::relations::dependent_relation<__name##_rel_t, __cdep> {          \
    template <class Origin> constexpr static bool check = __cond;              \
  };                                                                           \
  template <___dep> constexpr const static __name##_rel_t<__cdep> __name;

#define simp_simple_relation(__name, __cond)                                   \
  struct __name##_rel_t : simp::relations::relation<__name##_rel_t> {          \
    template <class Origin> constexpr static bool check = __cond;              \
  };                                                                           \
  constexpr const static __name##_rel_t __name;

} // namespace simp
