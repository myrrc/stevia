#include <utility>

namespace cond {

struct word_t {
  template <class T> constexpr bool calc() const { return true; }
};

template <class... Bases> struct inherits_t : word_t {
  constexpr inherits_t() = default;

  template <class T> constexpr bool calc() const {
    return (std::is_base_of_v<Bases, T> && ...);
  }
};

template <class ...Bases>
constexpr const static inherits_t<Bases...> inherits;

template <class T, template<class ...> class Word, class... Options>
    struct Condition {
  constexpr explicit Condition() = default;

  constexpr explicit operator bool() const {
    return (Word<Options...>{}).template calc<T>();
  }
};
}

template <class T>
struct _if_t {
  template <class ...Words>
  constexpr bool operator()(const Words& ...words) const {
    return (words.template calc<T>() && ...);
  }
};

template <class T>
constexpr const static _if_t<T> _if;

///

struct base1 {};
struct base2 {};
struct base3 {};
struct child : base1, base2 {};

using namespace cond;

int main() {
    static_assert(
    Condition<child, inherits_t, base1, base2>()
    && !Condition<child, inherits_t, base3>()
    );

  static_assert(
      _if<child>(
          inherits<base1>,
          inherits<base2>,
          inherits<base3>
          )
      );
}