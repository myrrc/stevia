#include "../../include/internal/relation_macro.h"
#include "../../include/stevia.h"
#include "gtest/gtest.h"

STEVIA_SIMPLE_RELATION(integral, (std::is_integral_v<Origin>))

using namespace stevia::internal;

using rel = integral_rel_t<void>;

TEST(internal_testing, extract_modifiers_none) {
    ASSERT_EQ(
    (extract_unique_modifiers(tl<true, rel>{})),
    tl<true>{});
}

TEST(internal_testing, extract_modifiers_one) {
    ASSERT_EQ(
        (extract_unique_modifiers(tl<true, mod_neg, rel>{})),
        (tl<true, mod_neg>{}));
}

TEST(internal_testing, extract_modifiers_many) {
    ASSERT_EQ(
        (extract_unique_modifiers((tl<true, rel, mod_or, rel, mod_or, mod_neg, rel, mod_or, mod_neg, rel>{}))),
        (tl<true, mod_or, mod_neg>{}));
}

