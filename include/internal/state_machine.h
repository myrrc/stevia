#pragma once

#include "core.h"
#include "modifiers.h"
#include "../../contrib/type_name.h"
#include "../../contrib/is_specialization.h"

namespace stevia::internal {

struct relation_base;

template <class Origin, class... Modifiers> struct state_machine {
    template <class... Cells> CONSTEXPR explicit state_machine(const tl<true, Cells...> &list) {
        STEVIA_LOG << "Initialized state machine for \"" << type_name<Origin>() << "\"\n" << sizeof...(Cells) << " steps\n";

        parse_tape(list, std::index_sequence_for<Cells...>{});
    }

    constexpr operator bool() const {return state;};

private:
    bool state{ true };

    tl<false, Modifiers...> contexts{};

    template <class Tape, size_t... I> CONSTEXPR void parse_tape(const Tape &tape, std::index_sequence<I...>) {
        ((parse_cell<
            typename std::remove_cv_t<
            typename std::remove_reference<decltype(get<I>(tape))>>
            ::type
            ::value_t>()
         ), ...);

        STEVIA_LOG << "\\->\tResult:\t" << state << "\n";
    }

    template <class Cell> CONSTEXPR void parse_cell() {
        if constexpr(is_specialization<Cell, modifier>::value) {
            find<Cell>(contexts).value.appeared();

            STEVIA_LOG << "|\t[Found modifier " << type_name<Cell>() << "]\n";

        } else if constexpr(std::is_base_of_v<relation_base, Cell>) {
            const bool relation_eval_result { parse_relation<Cell>() };

            STEVIA_LOG << "|\t" << relation_eval_result << " [" << type_name<Cell>() << "]\n";

            size_t modifiers_count{0};
            apply([&](auto& modifier) mutable { if (modifier.found) ++modifiers_count; }, contexts);

            if (modifiers_count == 0) {
                STEVIA_LOG << "|\t\t" << state << " -> ";
                state &= relation_eval_result;
                STEVIA_LOG << state << " [Applying &&]\n";

                return;
            }

            size_t modifiers_applied{0};

            bool intermediate_state{relation_eval_result};

            for (size_t prec = precedence_min; prec != precedence_max; ++prec) {
                apply([&](auto &modifier) mutable {
                    if (modifier.precedence != prec || !modifier.found) return;

                    STEVIA_LOG << "|\t\t ";

                    if (modifiers_applied != modifiers_count - 1) {
                        STEVIA_LOG << intermediate_state << " -> ";

                        if constexpr (modifier.unary) {
                            intermediate_state = modifier.evaluate(intermediate_state);
                        } else {
                            intermediate_state = modifier.evaluate(intermediate_state, relation_eval_result);
                        }

                        STEVIA_LOG << intermediate_state;
                    } else {
                        STEVIA_LOG << "final: " << state << " -> ";

                        if constexpr (modifier.unary) {
                            state &= modifier.evaluate(intermediate_state);
                        } else {
                            state = modifier.evaluate(state, intermediate_state);
                        }

                        STEVIA_LOG << state;
                    }

                  STEVIA_LOG << " [Applying " << type_name<decltype(modifier)>() << "]\n";

                    modifier.reset();
                    ++modifiers_applied;
                }, contexts);
            }
        }
    }

    template <class Relation>
    CONSTEXPR bool parse_relation() const {
        // TODO return storage.find<Origin, Dependent...>();
        return Relation{}.template check<Origin>;
    }
};

template <class OriginHolder, class... Modifiers, class... Cells>
CONSTEXPR bool evaluate(const tl<true, Modifiers...> &, const tl<true, Cells...> &tape) {
    return state_machine<typename OriginHolder::value_t, Modifiers...>(tape);
}

} // namespace stevia::state_machine