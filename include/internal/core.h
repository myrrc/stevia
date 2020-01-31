#pragma once

//#define STEVIA_DEBUG
#define STEVIA_LOG_FLOW

#ifdef STEVIA_DEBUG
#define CONSTEXPR inline
#else
#define CONSTEXPR constexpr
#endif

#ifdef STEVIA_LOG_FLOW

#include <iostream>
#include "../../contrib/type_name.h"

#define STEVIA_LOG std::cout

#else

#define STEVIA_LOG(expr)

#endif
