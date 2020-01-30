#include "utility"

#include "internal/guard_internal.h"
#include "internal/operators.h"

namespace simp::guard {

template <class... Origins, class... Cells> CONSTEXPR bool check(const tape<Cells...> &tape) {
    return guard_internal::check_impl<Origins...>(tape, std::index_sequence_for<Origins...>{});
}

template <class... Origins, class Relation,
          typename = std::enable_if_t<std::is_base_of_v<relations::relation_base, Relation>>>
CONSTEXPR bool check(const Relation &) {
    return check<Origins...>(tape<Relation>{});
}

template <class... Origins, class... Cells>
CONSTEXPR void assert(const tape<Cells...> &tape) {
    guard_internal::assert_helper<check<Origins...>(tape)>{};
}

template <class h, auto& Tape>
using where = std::enable_if_t<check_impl(h{}, Tape)>;

} // namespace simp