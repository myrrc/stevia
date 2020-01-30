#pragma once

#include "core.h"

#ifdef SIMP_LOG_FLOW
#include <iostream>
#include "../../contrib/type_name.h"
#endif

namespace simp::relations {
struct relation_base;
}

namespace simp::state_machine {

using typelist::tl;

template <class Origin, class... Modifiers> struct state_machine {
    template <class... Cells> CONSTEXPR explicit state_machine(const tl<true, Cells...> &list) {
        #ifdef SIMP_LOG_FLOW
        std::cout << "Initialized state machine for \""
                  << type_name<Origin>() << "\"\n"
                  << sizeof...(Cells) << " steps\n"
                  << "Cells tl: " << type_name<decltype(list)>()
                  << std::endl;
        #endif

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
            constexpr(std::is_base_of_v<modifiers::modifier_base, Cell>) {
                typelist::utils::find<Cell>(contexts).value.appeared();

                #ifdef SIMP_LOG_FLOW
                std::cout << "|\tFound modifier "
                          << type_name<Cell>()
                          << std::endl;
                #endif

                return;
            }
        else if
            constexpr(std::is_base_of_v<relations::relation_base, Cell>) {
                bool relation_eval_result = parse_relation<Cell>();

            #ifdef SIMP_LOG_FLOW
                std::cout << "|\tExpression :" << type_name<Cell>()
                      << " evaluated to " << std::boolalpha
                      << relation_eval_result
                      <<", state: " << state << " -> ";
            #endif

                typelist::utils::apply(
                    [this, &relation_eval_result](auto &modifier) CONSTEXPR mutable {
                    state = modifier.evaluate(state, relation_eval_result);
                    modifier.reset();
                }, contexts);

            #ifdef SIMP_LOG_FLOW
                std::cout << std::boolalpha << state << std::endl;
            #endif

                // TODO the bug is probably here
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