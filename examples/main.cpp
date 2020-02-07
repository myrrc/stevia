#include "../include/stevia.h"
#include "../include/internal/relation_macro.h"

stevia_make_relation(inherits, class ...B, B..., (std::is_base_of_v<B, Origin> && ...))
stevia_make_simple_relation(integral,(std::is_integral_v<Origin>))

struct base1 {};
struct base2 {};
struct base3 {};
struct child : base1, base2 {};
using child2 = child;

int main() {
    //stevia::check<child>(inherits<base1, base2> || inherits<base3>);
    //stevia::check<child>(inherits<base1, base2> || !inherits<base3>);
    //stevia::check<child>(!inherits<base1, base2> || inherits<base3>);
    stevia::check<child>(!inherits<base1, base2> && !inherits<base3>);
    //stevia::check<child>(!integral);
    //stevia::check<child>(integral);
    //stevia::assert<child>(inherits<base1, base2>);
}
