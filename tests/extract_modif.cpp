#include "../include/internal/relation_macro.h"
#include "../include/stevia.h"

stevia_make_simple_relation(integral, (std::is_integral_v<Origin>))

template <class Initial, class Got, class Expected>
struct tester {
    constexpr explicit tester() {
        static_assert(std::is_same_v<Got, Expected>);
    }
};

#define stevia_test(functor, initial, expected) \
tester<decltype(initial), decltype(functor(initial)), decltype(expected)>()


int main() {
    using namespace stevia::internal;

    using _or = modifier<mod_or_base_t>;
    using neg = modifier<mod_neg_base_t>;
    using rel = integral_rel_t<void>;

    stevia_test(extract_unique_modifiers,
                (tl<true, rel>{}),
                (tl<true>{}));

    stevia_test(extract_unique_modifiers,
        (tl<true, neg, rel>{}),
        (tl<true, neg>{}));

    stevia_test(extract_unique_modifiers,
        (tl<true, rel, _or, rel, _or, neg, rel, _or, neg, rel>{}),
        (tl<true, _or, neg>{}));
}
