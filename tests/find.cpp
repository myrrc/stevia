#include "../include/internal/relation_macro.h"
#include "../include/stevia.h"

stevia_make_simple_relation(integral,(std::is_integral_v<Origin>))

int main() {
    using namespace stevia::internal;

    constexpr auto list = tl<true,
        modifier<mod_neg_base_t>,
        integral_rel_t<void>
    >{};

    static_assert(std::is_same_v<
        decltype(find<modifier<mod_neg_base_t>>(list)),
        tl_holder<true, modifier<mod_neg_base_t>>
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

