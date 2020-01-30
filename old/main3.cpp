#include <utility>

namespace cond {

struct word_t {
  constexpr word_t() = default;
  constexpr word_t(bool c) : value(c) {}

  const bool value = true;

  template <class T> constexpr bool calc() const;

  constexpr word_t operator&&(const word_t &other) const {
    return value && other.value;
  }
  constexpr word_t operator||(const word_t &other) const {
    return value || other.value;
  }
  constexpr word_t operator!() const { return !value; }
};

template <class... Bases> struct inherits_t : word_t {
  template <class T> constexpr bool calc() const {
    return (std::is_base_of_v<Bases, T> && ...);
  }
};

template <class... Bases> constexpr const static inherits_t<Bases...> inherits;

struct _if_t {
  constexpr _if_t(bool value_ = true): value(value_) {}

  template <class T> constexpr _if_t make(const word_t &word) const {
    return {word.calc<T>()};
  }

  template <class T> constexpr _if_t _and(const word_t & word) const {
    return {value && word.calc<T>()};
  }

  constexpr operator bool() const {return value; }

  bool value;
};

template <class T, class... Words>
constexpr static _if_t _if(const Words &... words) {
  return _if_t().make<T>(words...);
}
}

///

struct base1 {};
struct base2 {};
struct base3 {};
struct child : base1, base2 {};
struct child2: base3 {};
struct child3 : base1 {};

using namespace cond;

int main() {

  static_assert(
      _if<child2>(inherits<base3> && !inherits<base1>)
      ._and<child>(inherits<base1, base2>)
  );
}