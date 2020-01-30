#pragma once

//#define DEBUG

#define SIMP_LOG_FLOW

#ifdef DEBUG

#define CONSTEXPR inline
#define CONSTEXPR_IF

#else

#define CONSTEXPR_IF constexpr
#define CONSTEXPR constexpr

#endif