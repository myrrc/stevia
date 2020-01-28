#pragma clang diagnostic push
#pragma ide diagnostic ignored "HidingNonVirtualFunction"
#pragma once

#include "core.h"
#include <cstddef>
#include <functional>

namespace simp::utils {

template <bool store, class T> struct dttl_store {};

template <class T> struct dttl_store<true, T> { T t; };

template <class T> struct dttl_store<false, T> {};

/**
 * Dependent types type list.
 * Can also store values.
 */
template <bool, class...> struct dttl { constexpr static size_t len{}; };

template <bool StoreValues> struct dttl<StoreValues> {
    template <class> CONSTEXPR static bool has{false};

    constexpr static size_t len = 0;

    template <class Target> CONSTEXPR dttl_store<false, Target &> find() { return {}; }

    template <class... OtherCells> using add = dttl<StoreValues, OtherCells...>;
};

template <bool StoreValues, template <class...> class Last, class... Dependent>
struct dttl<StoreValues, Last<Dependent...>> : dttl_store<StoreValues, Last<Dependent...>> {
private:
    using store_base = dttl_store<StoreValues, Last<Dependent...>>;

public:
    constexpr static size_t len = 1;

    template <class Target> CONSTEXPR static bool has = std::is_same_v<Target, Last<Dependent...>>;

    template <class Target>
    CONSTEXPR auto find() -> dttl_store<std::is_same_v<decltype(static_cast<store_base *>(this)), Target>, Target &> {
        auto *base = static_cast<store_base *>(this);
        if
            CONSTEXPR_IF(std::is_same_v<decltype(base->t), Target>) { return dttl_store<true, Target &>{base->t}; }
        return dttl_store<false, Target &>{};
    }

    template <class... OtherCells> using add = dttl<StoreValues, OtherCells..., Last<Dependent...>>;
};

template <bool StoreValues, template <class...> class Head, class... HeadDependent, class... Tail>
struct dttl<StoreValues, Head<HeadDependent...>, Tail...> : dttl_store<StoreValues, Head<HeadDependent...>>,
                                                            dttl<StoreValues, Tail...> {
private:
    using base = dttl<StoreValues, Tail...>;
    using store_base = dttl_store<StoreValues, Head<HeadDependent...>>;

public:
    constexpr static size_t len = 1 + sizeof...(Tail);

    template <class Target>
    CONSTEXPR static bool has = std::is_same_v<Target, Head<HeadDependent...>> ? true : base::template has<Target>;

    template <class Target>
    CONSTEXPR auto find() -> dttl_store<std::is_same_v<decltype(static_cast<store_base *>(this)), Target>, Target &> {
        auto *base = static_cast<store_base *>(this);
        if
            CONSTEXPR_IF(std::is_same_v<decltype(base->t), Target>) { return dttl_store<true, Target &>{base->t}; }
        return dttl_store<false, Target &>{};
    }

    template <class... OtherCells> using add = dttl<StoreValues, OtherCells..., Head<HeadDependent...>, Tail...>;
};

template <size_t I, class DTTL> struct get_type {};

template <size_t I, bool sv, class Type, class... Types> struct get_type<I, dttl<sv, Type, Types...>> {
    using value = get_type<I - 1, dttl<sv, Types...>>;
};

template <bool sv, class Type, class... Types> struct get_type<0, dttl<sv, Type, Types...>> { using value = Type; };

template <size_t I, class DTTL> using get_type_v = typename get_type<I, DTTL>::value;

template <size_t I, bool StoreValues, class Item, class... Others>
auto &get_value(const utils::dttl<StoreValues, Item, Others...> &target) {
    if
        CONSTEXPR_IF(I == 0) { return static_cast<typename decltype(target)::store_base &>(target).t; }

    return get_value<I - 1>(static_cast<typename decltype(target)::base &>(target));
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
template <class Functor, bool storeValues, class Cell>
CONSTEXPR auto extract_unique(Functor &&f, const utils::dttl<storeValues, Cell> &)
    -> std::conditional_t<
        std::is_same_v<
            std::true_type,
            std::invoke_result<Functor, Cell>
            >,
            utils::dttl<storeValues, Cell>, utils::dttl<storeValues>> {
    return {};
}
#pragma clang diagnostic pop

template <class Functor, bool storeValues, class Cell, class... Cells>
CONSTEXPR auto extract_unique(Functor &&, const utils::dttl<storeValues, Cell, Cells...> &dttl) -> std::conditional_t<
    std::is_same_v<std::true_type,
                   std::invoke_result<Functor, Cell>> || utils::dttl<storeValues, Cells...>::template has<Cell>,
    std::invoke_result_t<decltype(&extract_unique<Functor, storeValues, Cells...>), utils::dttl<storeValues, Cells...>>,
    typename std::invoke_result_t<decltype(&extract_unique<Functor, storeValues, Cells...>),
                                  utils::dttl<storeValues, Cells...>>::template add<Cell>> {
    return {};
}
} // namespace simp::utils

#pragma clang diagnostic pop