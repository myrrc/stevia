#pragma once

namespace { template <class...> struct ne {}; }

template <class ...T>
using enable_if_not_empty = std::enable_if_t<!std::is_same_v<ne<>, ne<T...>>>;