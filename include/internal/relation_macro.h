#pragma once

#define simp_relation(__name, ___dep, __cdep, __cond)                                                                  \
    template <___dep> struct __name##_rel_t : simp::relations::relation<__name##_rel_t, __cdep> {            \
        template <class Origin> CONSTEXPR static bool check = __cond;                                           \
    };                                                                                                                 \
    template <___dep> CONSTEXPR const static __name##_rel_t<__cdep> __name;

#define simp_simple_relation(__name, __cond) \
    template <class>\
    struct __name##_rel_t : simp::relations::relation<__name##_rel_t, void> {                                                \
        template <class Origin> CONSTEXPR static bool check = __cond;                                           \
    };                                                                                                                 \
    CONSTEXPR const static __name##_rel_t<void> __name;
