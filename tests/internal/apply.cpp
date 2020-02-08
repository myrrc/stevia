#include "../../include/stevia.h"
#include "gtest/gtest.h"

TEST(internal_testing, apply_many) {
    using namespace stevia::internal;

    tl<false,
        modifier<mod_neg_base_t>,
        modifier<mod_or_base_t>
    > list;

    int count{0};

    ASSERT_FALSE(get<0>(list).value.found);
    ASSERT_FALSE(get<1>(list).value.found);

    get<1>(list).value.appeared();

    ASSERT_FALSE(get<0>(list).value.found);
    ASSERT_TRUE(get<1>(list).value.found);

    apply([&count](auto& cell) mutable {
      ++count;

      cell.reset();

      ASSERT_FALSE(cell.found);
    }, list);

    ASSERT_EQ(count, 2);
}
