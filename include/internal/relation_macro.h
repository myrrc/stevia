#pragma once

#define STEVIA_RELATION(__name, ___dep, __cdep, __cond)                                                                  \
    template <___dep> struct __name##_rel_t : stevia::internal::relation<__name##_rel_t, __cdep> {            \
        template <class Origin> CONSTEXPR static bool check = __cond;                                           \
    };                                                                                                                 \
    template <___dep> CONSTEXPR const static __name##_rel_t<__cdep> __name;

#define STEVIA_SIMPLE_RELATION(__name, __cond) \
    template <class>\
    struct __name##_rel_t : stevia::internal::relation<__name##_rel_t, void> {                                                \
        template <class Origin> CONSTEXPR static bool check = __cond;                                           \
    };                                                                                                                 \
    CONSTEXPR const static __name##_rel_t<void> __name;
