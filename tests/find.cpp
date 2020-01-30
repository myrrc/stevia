#include "../include/guard.h"
#include "../include/internal/relation_macro.h"

simp_simple_relation(integral,(std::is_integral_v<Origin>))

int main() {
    using simp::typelist::tl;
    using simp::modifiers::relation_modifier;
    using simp::modifiers::mod_neg_base_t;
    using simp::typelist::utils::find;
    using simp::typelist::tl_holder;

    constexpr auto list = tl<true,
        relation_modifier<mod_neg_base_t>,
        integral_rel_t<void>
    >{};

    static_assert(std::is_same_v<
        decltype(find<relation_modifier<mod_neg_base_t>>(list)),
        tl_holder<true, relation_modifier<mod_neg_base_t>>
    >);

    static_assert(std::is_same_v<
        decltype(find<integral_rel_t<void>>(list)),
        tl_holder<true, integral_rel_t<void>>
    >);

    static_assert(std::is_same_v<
        decltype(find<int>(list)),
        decltype(tl_holder<false, int>{})
        >);
}

