#include <utility>

namespace cond {

struct word_t {
  template <class T> constexpr static const bool calc = true;
};

template <class... Bases> struct inherits_t : word_t {
  template <class T> constexpr static const bool calc =
    (std::is_base_of_v<Bases, T> && ...);
};

template <class ...Bases>
constexpr const static inherits_t<Bases...> inherits;
}

template <class T>
struct _if_t {
  template <class ...Words>
  constexpr bool operator()(const Words& ...words) const {
    return (words.template calc<T> && ...);
  }
};

template <class T>
constexpr const static _if_t<T> _if;

struct _mif_t {
  constexpr _mif_t(bool val = true): value(val) {}

  constexpr explicit operator bool() const { return value;}

  template <class T, class ...Words>
  constexpr _mif_t _and(const Words& ...words) const {
    return value && (words.template calc<T> && ...);
  }

  template <class T, class ...Words>
  constexpr _mif_t _or(const Words& ...words) const {
    return value || (words.template calc<T> && ...);
  }

  const bool value;
};

constexpr const static _mif_t _mif;

///

struct base1 {};
struct base2 {};
struct base3 {};
struct child : base1, base2 {};
struct child2: base3 {};

using namespace cond;

int main() {
  static_assert(
      _mif
          ._and<child>(inherits<base1, base2>)
          ._or<child2>(inherits<base3>)
              )
  );
}