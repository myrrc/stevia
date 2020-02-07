#include "../include/internal/relation_macro.h"
#include "../include/stevia.h"

stevia_make_simple_relation(integral,(std::is_integral_v<Origin>))

int main() {
    using namespace stevia::internal;

    tl<false,
        modifier<mod_neg_base_t>,
        modifier<mod_or_base_t>
    > list;

    int count = 0;
    get<0>(list).value.appeared();

    apply([&count](auto& cell) mutable {
        //using modif_t = typename std::remove_reference_t<holder_t>::type;

      STEVIA_LOG << type_name<decltype(cell)>() << "\n"
                 << cell.found << "\n";

      ++count;

      cell.reset();

      STEVIA_LOG << cell.found << "\n";
    }, list);

    std::cout << count;
}
