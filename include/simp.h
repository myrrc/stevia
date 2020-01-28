#pragma clang diagnostic push
#pragma ide diagnostic ignored "NotImplementedFunctions"
#pragma ide diagnostic ignored "HidingNonVirtualFunction"
#pragma ide diagnostic ignored "modernize-use-nodiscard"

#include "utility"
#include <cstddef>
#include <type_traits>

#include "dttl.h"

namespace simp {

template <class... Args> using tape = utils::dttl<false, sizeof...(Args), Args...>;
template <class... Args> using context_storage = utils::dttl<true, sizeof...(Args), Args...>;

namespace modifiers {
struct modifier_base {};
template <class Modifier> struct relation_modifier : modifier_base {
    bool found{false};

    constexpr void appeared() { found = true; }
    constexpr void reset() { found = false; }

    constexpr bool evaluate(bool state, bool relation_result) const;
};


struct mod_or_base_t{};

template <>
struct relation_modifier<mod_or_base_t> {
    bool found{false};

    constexpr void appeared() { found = true; }
    constexpr void reset() { found = false; }

    constexpr bool evaluate(bool state, bool relation_result) const {
        return found ? state || relation_result : state && relation_result;
    }
};

using mod_or = relation_modifier<mod_or_base_t>;

struct mod_neg_base_t{};

template<>
struct relation_modifier<mod_neg_base_t> {
    bool found{false};

    constexpr void appeared() { found = true; }
    constexpr void reset() { found = false; }

    constexpr bool evaluate(bool state, bool relation_result) const {
        return state && found == !relation_result;
    }
};

using mod_neg = relation_modifier<mod_neg_base_t>;

} // namespace modifiers

namespace extractor {

template <typename... Types>
struct typelist {};

template <class Cell>
constexpr auto extract_modifiers_impl()
-> std::conditional_t<std::is_base_of_v<modifiers::modifier_base, Cell>, tape<Cell>, tape<>>
{ return {}; }

template <class Cell, class ...Cells,
    typename = std::enable_if_t<!
        std::is_same_v<typelist<Cells...>, typelist<>>>>
constexpr auto extract_modifiers_impl()
{
    auto tail_mod_list = extract_modifiers_impl<Cells...>();

    if constexpr (!std::is_base_of_v<modifiers::modifier_base, Cell>) { // not a modifier
        return tail_mod_list;
    } else if constexpr (decltype(tail_mod_list)::template has<Cell>) { // already exists in the list
        return tail_mod_list;
    }

    using t = decltype(tail_mod_list);
    return typename t::template add<Cell>();
}

template <class... Cells>
constexpr auto extract_modifiers(const tape<Cells...> &) { // TODO write general_function extract(func, tape)
    return extract_modifiers_impl<Cells...>();
}

}

namespace relations {
struct relation_base {};
} // namespace relations

namespace state_machine {

template <class Origin, class... Modifiers> struct state_machine {
    template <class Tape> constexpr explicit state_machine(const Tape &tape) {
        parse_tape(tape, std::index_sequence_for<typename Tape::len>{});
    }

private:
    context_storage<Modifiers...> contexts{};

    bool state{true};

    template <class Tape, size_t... I> constexpr void parse_tape(const Tape &tape, std::index_sequence<I...>) {
        (static_cast<void>(parse_cell<typename Tape::template get_type<I>>()), ...);
    }

    template <class Functor, size_t... I> void apply_to_modifiers(Functor &&functor, std::index_sequence<I...>) {
        (static_cast<void>(functor(contexts.template get_value<I>())), ...);
    }

    template <class Cell> constexpr void parse_cell() {
        if constexpr (std::is_base_of_v<modifiers::modifier_base, Cell>) {
            contexts.template find<Cell>().t.appeared();
            return;
        }

        if constexpr (std::is_base_of_v<relations::relation_base, Cell>) {
            constexpr bool relation_eval_result = parse_relation<Cell>();

            apply_to_modifiers(
                [this, &relation_eval_result](auto &modifier) mutable {
                    state = modifier.evaluate(state, relation_eval_result);
                    modifier.reset();
                },
                std::index_sequence_for<Modifiers...>{});
        }
    }

    template <template <class...> class DepRel, class... Dependent> constexpr bool parse_relation() const {
        // TODO Replace
        return DepRel<Dependent...>{}.template check<Origin>;
        // return storage.find<Origin, Dependent...>();
    }

    template <class Relation> constexpr bool parse_relation() const {
        // TODO Replace
        return Relation{}.template check<Origin>;
        // return storage.find<Origin, Dependent...>();
    }
};

template <class Origin, class... Modifiers>
constexpr auto make_state_machine(const utils::dttl<false, sizeof...(Modifiers), Modifiers...> &) {
    return state_machine<Origin, Modifiers...>{};
}
} // namespace state_machine

namespace relations {

template <template <class...> class Relation, class... Dependent> struct dependent_relation : relation_base {
    using type = Relation<Dependent...>;

    constexpr auto operator!() const { return tape<modifiers::mod_neg, type>{}; }

    template <template <class...> class OtherRel, class... OtherDep,
              typename = std::enable_if_t<std::is_base_of_v<relation_base, OtherRel>>>
    constexpr auto operator&&(const OtherRel<OtherDep...> &) const {
        return tape<type, OtherRel<OtherDep>...>{};
    }

    template <template <class...> class OtherRel, class... OtherDep,
              typename = std::enable_if_t<std::is_base_of_v<relation_base, OtherRel>>>
    constexpr auto operator||(const OtherRel<OtherDep...> &) const {
        return tape<type, modifiers::mod_or, OtherRel<OtherDep...>>{};
    }

    template <class OtherRel, typename = std::enable_if_t<std::is_base_of_v<relation_base, OtherRel>>>
    constexpr auto operator&&(const OtherRel &other) const {
        return tape<type, OtherRel>{};
    }

    template <class OtherRel, typename = std::enable_if_t<std::is_base_of_v<relation_base, OtherRel>>>
    constexpr auto operator||(const OtherRel &other) const {
        return tape<type, OtherRel>{};
    }
};

template <class Relation> struct relation : relation_base {
    using type = Relation;

    constexpr auto operator!() const { return tape<modifiers::mod_neg, type>{}; }

    template <template <class...> class OtherRel, class... OtherDep,
              typename = std::enable_if_t<std::is_base_of_v<relation_base, OtherRel>>>
    constexpr auto operator&&(const OtherRel<OtherDep...> &) const {
        return tape<type, OtherRel<OtherDep>...>{};
    }

    template <template <class...> class OtherRel, class... OtherDep,
              typename = std::enable_if_t<std::is_base_of_v<relation_base, OtherRel>>>
    constexpr auto operator||(const OtherRel<OtherDep...> &) const {
        return tape<type, modifiers::mod_or, OtherRel<OtherDep...>>{};
    }

    template <class OtherRel, typename = std::enable_if_t<std::is_base_of_v<relation_base, OtherRel>>>
    constexpr auto operator&&(const OtherRel &other) const {
        return tape<type, OtherRel>{};
    }

    template <class OtherRel, typename = std::enable_if_t<std::is_base_of_v<relation_base, OtherRel>>>
    constexpr auto operator||(const OtherRel &other) const {
        return tape<type, OtherRel>{};
    }
};

} // namespace relations

namespace operators {

template <template <class...> class Relation, class... Dep, class... OtherCells>
static constexpr auto operator&&(const Relation<Dep...> &left, const tape<OtherCells...> &right) {
    return tape<Relation<Dep...>, OtherCells...>{};
}

template <template <class...> class Relation, class... Dep, class... OtherCells>
static constexpr auto operator||(const Relation<Dep...> &left, const tape<OtherCells...> &right) {
    return tape<Relation<Dep...>, modifiers::mod_or, OtherCells...>{};
}

template <template <class...> class Relation, class... Dep, class... OtherCells>
static constexpr auto operator&&(const tape<OtherCells...> &left, const Relation<Dep...> &right) {
    return tape<OtherCells..., Relation<Dep...>>{};
}

template <template <class...> class Relation, class... Dep, class... OtherCells>
static constexpr auto operator||(const tape<OtherCells...> &left, const Relation<Dep...> &right) {
    return tape<OtherCells..., modifiers::mod_or, Relation<Dep...>>{};
}
} // namespace operators

template <class... Origins, class... Cells, size_t... I>
constexpr bool check_impl(const tape<Cells...> &tape, [[maybe_unused]] std::index_sequence<I...>) {
    using origins_types_list = utils::dttl<false, sizeof...(Origins), Origins...>;
    auto tape_modificators_list = extractor::extract_modifiers(tape);

    return (
        // TODO write type iterator
        state_machine::make_state_machine<typename origins_types_list::template get_type<I>>(tape_modificators_list)
        && ... && true);
}

namespace checks {
template <class... Origins, class... Cells> constexpr bool check(const tape<Cells...> &tape) {
    return check_impl<Origins...>(tape, std::index_sequence_for<Origins...>{});
}

template <class... Origins, class Relation,
          typename = std::enable_if_t<std::is_base_of_v<relations::relation_base, Relation>>>
constexpr bool check(const Relation &) {
    return check<Origins...>(tape<Relation>{});
}
} // namespace checks
using namespace checks;

#define simp_relation(__name, ___dep, __cdep, __cond)                                                                  \
    template <___dep> struct __name##_rel_t : simp::relations::dependent_relation<__name##_rel_t, __cdep> {            \
        template <class Origin> constexpr static bool check = __cond;                                                  \
    };                                                                                                                 \
    template <___dep> constexpr const static __name##_rel_t<__cdep> __name;

#define simp_simple_relation(__name, __cond)                                                                           \
    struct __name##_rel_t : simp::relations::relation<__name##_rel_t> {                                                \
        template <class Origin> constexpr static bool check = __cond;                                                  \
    };                                                                                                                 \
    constexpr const static __name##_rel_t __name;

} // namespace simp

using namespace simp::operators;

#pragma clang diagnostic pop