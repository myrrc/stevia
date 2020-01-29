#include "utility"

#include "checks.h"
#include "operators.h"

namespace simp::guard {

template <class... Origins, class... Cells> CONSTEXPR bool check(const tape<Cells...> &tape) {
    return checks_internal::check_impl<Origins...>(tape, std::index_sequence_for<Origins...>{});
}

template <class... Origins, class Relation,
          typename = std::enable_if_t<std::is_base_of_v<relations::relation_base, Relation>>>
CONSTEXPR bool check(const Relation &) {
    return check<Origins...>(tape<Relation>{});
}

#define simp_relation(__name, ___dep, __cdep, __cond)                                                                  \
    template <___dep> struct __name##_rel_t : simp::relations::dependent_relation<__name##_rel_t, __cdep> {            \
        template <class Origin> CONSTEXPR static bool check = __cond;                                           \
    };                                                                                                                 \
    template <___dep> CONSTEXPR const static __name##_rel_t<__cdep> __name;

#define simp_simple_relation(__name, __cond)                                                                           \
    struct __name##_rel_t : simp::relations::relation<__name##_rel_t> {                                                \
        template <class Origin> CONSTEXPR static bool check = __cond;                                           \
    };                                                                                                                 \
    CONSTEXPR const static __name##_rel_t __name;

} // namespace simp