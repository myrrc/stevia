#include "include/simp.h"

simp_relation(inherits, class ...B, B..., (std::is_base_of_v<B, Origin> && ...))
simp_simple_relation(is_integral,(std::is_integral_v<Origin>))

struct base1 {};
struct base2 {};
struct base3 {};
struct child : base1, base2 {};
using child2 = child;

int main() {
  static_assert(simp::check<child, child2>(inherits<base1, base2> && !inherits<base3>));
  static_assert(simp::check<base1, int, float, short, long, bool>(is_integral));
}