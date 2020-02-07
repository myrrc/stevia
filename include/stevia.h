#include "utility"

#include "internal/impl.h"
#include "internal/operators.h"
#include "../contrib/is_specialization.h"

namespace stevia {

template <class... Origins, class... Cells> CONSTEXPR bool check(const internal::tl<true, Cells...> &tape) {
    return internal::check_impl<true, Origins...>(tape, std::index_sequence_for<Origins...>{});
}

template <class... Origins, class Relation,
          typename = std::enable_if_t<std::is_base_of_v<internal::relation_base, Relation>>>
CONSTEXPR bool check(const Relation &) {
    return check<Origins...>(internal::tl<true, Relation>{});
}

template <class... Origins, class... Cells> CONSTEXPR bool check_any(const internal::tl<true, Cells...> &tape) {
    return internal::check_impl<false, Origins...>(tape, std::index_sequence_for<Origins...>{});
}

template <class... Origins, class Relation,
    typename = std::enable_if_t<std::is_base_of_v<internal::relation_base, Relation>>>
CONSTEXPR bool check_any(const Relation &) {
    return check_any<false, Origins...>(internal::tl<true, Relation>{});
}

template <class... Origins, class... Cells> CONSTEXPR void assert(const internal::tl<true, Cells...> &tape) {
    internal::assert_helper<check<Origins...>(tape)>{};
}

template <class... Origins, class Relation, typename = std::enable_if_t<std::is_base_of_v<internal::relation_base, Relation>>>
CONSTEXPR void assert(const Relation &) {
    internal::assert_helper<check<Origins...>(internal::tl<true, Relation>{})>{};
}

template <class... Origins, class... Cells> CONSTEXPR void assert_any(const internal::tl<true, Cells...> &tape) {
    internal::assert_helper<check_any<Origins...>(tape)>{};
}

template <class... Origins, class Relation, typename = std::enable_if_t<std::is_base_of_v<internal::relation_base, Relation>>>
CONSTEXPR void assert_any(const Relation &) {
    internal::assert_helper<assert_any<Origins...>(internal::tl<true, Relation>{})>{};
}

template <class h, auto& Tape>
using where = std::enable_if_t<check(h{}, Tape)>;

template <class h, auto& Tape>
using where_any = std::enable_if_t<check_any(h{}, Tape)>;
} // namespace stevia