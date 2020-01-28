#include <utility>

namespace cond {
/// Type = C++ type -- class1, int, ...
/// Word = special command -- inherits, are_same, ...
/// Condition = (Word, Type...) -- types_list -> inherits<class2, class3>
/// Compile-time knowledge (Type..., Condition..., bool)

/// _if<Type>(Condition{})
/// (Type, Condition1 && !Condition2 || ...)

/// _if<child1>(inherits<base1, base2> && !inherits<base3>)
/// _if<child1, child2, int>(convertible_to<bool>)

/// No macros, little boilerplate

/// (Type... Condition..., bool) ->
/// (Type, Condition..., bool)... ->

/// check<child1, inherits, base1, base2, and_t, not_t, inherits, base_3>
/// check<child1, convertible_to, bool>
/// check<child2, convertible_to, bool>
/// check<child3, convertible_to, bool>

/// check<Target, [[modificator... | None] word [Dependent...]]>
/// check_single<Target, eModificatorBitMask, word,  [Dependent...]>
/// check_single_wom<Target, word, ...Dependent> -> bool r; return modif(r);

/// Compile-time storage for Words
/// add<
/// Cond, // true or false
/// Original...
/// Dependent...
/// >
///
/// find<Original..., dependent...> -> Cond

enum class eModificatorBitMask {
  eAnd = 1,
  eOr = 1 << 1,
  eXor = 1 << 2,
  eNeg = 1 << 3
};

using mod = eModificatorBitMask;

static constexpr const auto and_m {mod::eAnd };
static constexpr const auto or_m {mod::eOr };
static constexpr const auto xor_m {mod::eXor };
static constexpr const auto nwg_m {mod::eNeg };

template <class Target, class ...ModWOrdDep>
constexpr bool check_single_target_sindle_word();

template <class Target, class ...ModWordDep>
constexpr bool check_single_target();

template <class Target, mod modifier, class Word, class ...Dependent>
constexpr bool check_single_target_single_word_parsed_modifier();

template <class Target, class Word, class ...Dependent>
constexpr bool check_single_target_single_word_no_mod();
}