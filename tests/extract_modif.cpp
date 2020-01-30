#include "../include/guard.h"
#include "../include/internal/relation_macro.h"

simp_simple_relation(integral,(std::is_integral_v<Origin>))

int main() {
    using simp::typelist::tl;
    using simp::modifiers::relation_modifier;
    using simp::modifiers::mod_neg_base_t;
    using simp::typelist::utils::extract_unique_modifiers;

    constexpr auto list = tl<true,
        relation_modifier<mod_neg_base_t>,
        integral_rel_t<void>
    >{};

    [[maybe_unused]] constexpr auto um {extract_unique_modifiers(list)};
}
