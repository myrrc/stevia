#pragma once

#include "core.h"

namespace simp::relations {
struct relation_base;
}

namespace simp::state_machine {

template <class Origin, class... Modifiers> struct state_machine {
    template <class... Cells> CONSTEXPR explicit state_machine(const utils::dttl<false, Cells...> &) {
        parse_tape<utils::dttl<false, Cells...>>(
            std::make_index_sequence<::simp::utils::dttl<false, Cells...>::len>{});
    }

    bool state{true};

private:
    utils::dttl<true, Modifiers...> contexts{};

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedTemplateParameterInspection"
    template <class Tape, size_t... I> CONSTEXPR void parse_tape(std::index_sequence<I...>) {
        (parse_cell<utils::get_type_v<I, Tape>>(), ...);
    }
#pragma clang diagnostic pop

    template <class Functor, size_t... I>
    CONSTEXPR void apply_to_modifiers(Functor &&functor, std::index_sequence<I...>) {
        (functor(contexts.template get_value<I>()), ...);
    }

    template <class Cell> CONSTEXPR void parse_cell() {
        if CONSTEXPR_IF (std::is_base_of_v<modifiers::modifier_base, Cell>) {
            //contexts.template find<Cell>().t.appeared();
            return;
        }

        if CONSTEXPR_IF (std::is_base_of_v<relations::relation_base, Cell>) {
            bool relation_eval_result = parse_relation<Cell>();

            apply_to_modifiers(
                [this, &relation_eval_result](auto &modifier) mutable {
                    state = modifier.evaluate(state, relation_eval_result);
                    modifier.reset();
                },
                std::index_sequence_for<Modifiers...>{});
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

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
template <class Origin, class... Modifiers, class... Cells>
CONSTEXPR bool evaluate(const utils::dttl<false, Modifiers...> &modifiers, const utils::dttl<false, Cells...> &tape) {
    return state_machine<Origin, Modifiers...>(tape).state;
}
#pragma clang diagnostic pop
} // namespace simp::state_machine