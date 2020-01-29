#pragma once

#include "core.h"

namespace simp::relations {
struct relation_base;
}

namespace simp::state_machine {

using typelist::tl;

template <class Origin, class... Modifiers> struct state_machine {
    template <class... Cells> CONSTEXPR explicit state_machine(const tl<true, Cells...> &list) {
        parse_tape(list, std::index_sequence_for<Cells...>{});
    }

    bool state{true};

private:
    tl<false, Modifiers...> contexts{};

    template <class Tape, size_t... I> CONSTEXPR void parse_tape(const Tape &tape, std::index_sequence<I...>) {
        ((parse_cell<typename decltype(typelist::utils::get<I>(tape))::type>()), ...);
    }

    template <class Cell> CONSTEXPR void parse_cell() {
        if
            CONSTEXPR_IF(std::is_base_of_v<modifiers::modifier_base, Cell>) {
                typelist::utils::find<Cell>(contexts).value.appeared();
                return;
            }
        else if
            CONSTEXPR_IF(std::is_base_of_v<relations::relation_base, Cell>) {
                bool relation_eval_result = parse_relation<Cell>();

                typelist::utils::apply(
                    [this, &relation_eval_result](auto &modifier) CONSTEXPR mutable {
                        state = modifier.evaluate(state, relation_eval_result);
                        modifier.reset();
                    },
                    contexts);
            }
    }

    template <template <class...> class DepRel, class... Dependent>[[nodiscard]] CONSTEXPR bool parse_relation() const {
        // TODO Replace
        return DepRel<Dependent...>{}.template check<Origin>;
        // return storage.find<Origin, Dependent...>();
    }

    template <class Relation>[[nodiscard]] CONSTEXPR bool parse_relation() const {
        // TODO Replace
        return Relation{}.template check<Origin>;
        // return storage.find<Origin, Dependent...>();
    }
};

template <class OriginHolder, class... Modifiers, class... Cells>
CONSTEXPR bool evaluate(const tl<true, Modifiers...> &, const tl<true, Cells...> &tape) {
    return state_machine<typename OriginHolder::type, Modifiers...>(tape).state;
}

} // namespace simp::state_machine