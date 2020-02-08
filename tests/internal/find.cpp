#include "../../include/internal/relation_macro.h"
#include "../../include/stevia.h"
#include "../../include/internal/tl.h"
#include "gtest/gtest.h"

using namespace stevia;
using namespace stevia::internal;

STEVIA_SIMPLE_RELATION(integral,(std::is_integral_v<Origin>))

TEST(internal_testing, find_empty) {
    constexpr auto list = tl<true>{};
    ASSERT_EQ(find<mod_or>(list), (elem_not_found<true, mod_or>));
}

TEST(internal_testing, find_one) {
    constexpr auto list = tl<true, mod_or>{};
    ASSERT_EQ(find<mod_or>(list), (tl_holder<true, mod_or>{}));
    ASSERT_EQ(find<mod_neg>(list), (elem_not_found<true, mod_neg>));
}

TEST(internal_testing, find_many) {
    constexpr auto list = tl<true, mod_neg, integral_rel_t<void>>{};

    ASSERT_EQ(find<mod_neg>(list),
              (tl_holder<true, mod_neg>{}));

    ASSERT_EQ(find<integral_rel_t<void>>(list),
              (tl_holder<true, integral_rel_t<void>>{}));

    ASSERT_EQ(find<int>(list),
              (elem_not_found<true, int>));
}