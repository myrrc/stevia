#pragma once

#define stevia_make_relation(__name, ___dep, __cdep, __cond)                                                                  \
    template <___dep> struct __name##_rel_t : simp::internal::relation<__name##_rel_t, __cdep> {            \
        template <class Origin> CONSTEXPR static bool check = __cond;                                           \
    };                                                                                                                 \
    template <___dep> CONSTEXPR const static __name##_rel_t<__cdep> __name;

#define stevia_make_simple_relation(__name, __cond) \
    template <class>\
    struct __name##_rel_t : stevia::internal::relation<__name##_rel_t, void> {                                                \
        template <class Origin> CONSTEXPR static bool check = __cond;                                           \
    };                                                                                                                 \
    CONSTEXPR const static __name##_rel_t<void> __name;
