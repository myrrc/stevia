#pragma once

//#define DEBUG

#ifdef DEBUG

#define CONSTEXPR inline
#define CONSTEXPR_IF

#else

#define CONSTEXPR_IF constexpr
#define CONSTEXPR constexpr

#endif