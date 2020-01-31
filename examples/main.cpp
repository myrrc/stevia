#include "../include/stevia.h"
#include "../include/internal/relation_macro.h"

simp_relation(inherits, class ...B, B..., (std::is_base_of_v<B, Origin> && ...))
simp_simple_relation(integral,(std::is_integral_v<Origin>))

struct base1 {};
struct base2 {};
struct base3 {};
struct child : base1, base2 {};
using child2 = child;

int main() {
    bool res = simp::guard::check<child>(!inherits<base3>);
    return res ? 0 : 99;

    //static_assert(simp::check<child, child2>(inherits<base1, base2> && !inherits<base3>));

    //simp::guard::assert<child>(inherits<base1, base2>);
}