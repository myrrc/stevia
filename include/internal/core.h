#pragma once

//#define DEBUG

#define SIMP_LOG_FLOW

#ifdef DEBUG
#define CONSTEXPR inline
#else
#define CONSTEXPR constexpr
#endif