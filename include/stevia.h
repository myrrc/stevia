#include "utility"

#include "internal/impl.h"
#include "internal/operators.h"
#include "../contrib/is_specialization.h"

namespace stevia {

template <class... Origins, class... Cells> CONSTEXPR bool check(const internal::tl<true, Cells...> &tape) {
    return internal::check_impl<Origins...>(tape, std::index_sequence_for<Origins...>{});
}

template <class... Origins, class Relation,
    typename = std::enable_if_t<
        std::is_base_of_v<internal::relation_base, Relation>>>
CONSTEXPR bool check(const Relation &) {
    return check<Origins...>(internal::tl<true, Relation>{});
}

template <class... Origins, class... Cells>
CONSTEXPR void assert(const internal::tl<true, Cells...> &tape, std::string_view message = "Assertion failed") {
    internal::assert_helper<check<Origins...>(tape)>{message};
}

template <class... Origins, class Relation,
    typename = std::enable_if_t<std::is_base_of_v<internal::relation_base, Relation>>>
CONSTEXPR void assert(const Relation &, std::string_view message = "Assertion failed") {
    internal::assert_helper<check<Origins...>(internal::tl<true, Relation>{})>{message};
}

template <class h, auto& Tape>
using where = std::enable_if_t<check_impl(h{}, Tape)>;

} // namespace stevia