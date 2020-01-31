#include "../include/internal/relation_macro.h"
#include "../include/stevia.h"

stevia_make_simple_relation(integral, (std::is_integral_v<Origin>))

#define stevia_test(name, functor, initial, expected)                                                                  \
    static_assert((std::is_same_v<decltype(functor(initial)), decltype(expected)>), "Test \"" name "\" failed")


int main() {
    using namespace stevia::internal;

    using _or = modifier<mod_or_base_t>;
    using neg = modifier<mod_neg_base_t>;
    using rel = integral_rel_t<void>;

    stevia_test("!integral", extract_unique_modifiers, (tl<true, _or, rel>{}), (tl<true, _or>{}));

    stevia_test("integral || integral || !integral || !integral", extract_unique_modifiers,
        (tl<true, rel, _or, rel, _or, neg, rel, _or, neg, rel>{}), (tl<true, _or, neg>{}));
}
