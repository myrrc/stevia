#pragma once

#include "core.h"
#include "modifiers.h"

namespace stevia::internal {

struct relation_base;

template <class Origin, class... Modifiers> struct state_machine {
    template <class... Cells> CONSTEXPR explicit state_machine(const tl<true, Cells...> &list) {
        STEVIA_LOG << "Initialized state machine for \""
                 << type_name<Origin>() << "\"\n"
                 << sizeof...(Cells) << " steps\n"
                 << "Cells tl: " << type_name<decltype(list)>()
                 << std::endl;

        parse_tape(list, std::index_sequence_for<Cells...>{});
    }

    bool state{true};

private:
    tl<false, Modifiers...> contexts{};

    template <class Tape, size_t... I> CONSTEXPR void parse_tape(const Tape &tape, std::index_sequence<I...>) {
        ((parse_cell<typename decltype(get<I>(tape))::type>()), ...);
    }

    template <class Cell> CONSTEXPR void parse_cell() {
        if constexpr(std::is_base_of_v<modifier_base, Cell>) {
                find<Cell>(contexts).value.appeared();

                STEVIA_LOG << "|\tFound modifier " << type_name<Cell>() << std::endl;

                return;
        } else if constexpr(std::is_base_of_v<relation_base, Cell>) {
            bool relation_eval_result = parse_relation<Cell>();

            STEVIA_LOG << "|\tExpression :" << type_name<Cell>()
                     << " evaluated to " << std::boolalpha
                     << relation_eval_result
                     <<", state: " << state << " -> ";

            apply( // TODO BUG: state not updating
                [this, &relation_eval_result](auto &modifier) CONSTEXPR mutable {
                    state = modifier.evaluate(state, relation_eval_result);
                    modifier.reset();
                    }, contexts);

            STEVIA_LOG << std::boolalpha << state << std::endl;
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

} // namespace stevia::state_machine