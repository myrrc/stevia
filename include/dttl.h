#pragma once

namespace simp::utils {

template <bool store, class T> struct dttl_store {};

template <class T> struct dttl_store<true, T> { T t; };

template <class T> struct dttl_store<false, T> {};

/**
 * Dependent types type list.
 * Can also store values.
 */
template <bool, size_t, class...> struct dttl { };

template <bool StoreValues, size_t Length>
struct dttl<StoreValues, Length> {
    template <class _>
    constexpr static bool has {false};

    template <class... OtherCells>
    using add = dttl<StoreValues, Length + sizeof...(OtherCells), OtherCells...>;
};


template <bool StoreValues, size_t Length, template <class...> class Last, class... Dependent>
struct dttl<StoreValues, Length, Last<Dependent...>> : dttl_store<StoreValues, Last<Dependent...>> {
private:
    using store_base = dttl_store<StoreValues, Last<Dependent...>>;

public:
    [[maybe_unused]] constexpr const static size_t len = Length;

    template <class Target>
    constexpr static bool has = std::is_same_v<Target, Last>;

    template <size_t Index, typename = std::enable_if_t<Index == Length - 1>> using get_type = Last<Dependent...>;

    template <size_t Index, typename = std::enable_if_t<(Index == Length - 1) && StoreValues>>
    constexpr auto &get_value() {
        return static_cast<store_base *>(this)->t;
    }

    template <class Target, bool Found, typename = std::enable_if_t<StoreValues>>
    constexpr dttl_store<Found, Target &> find() {
        auto *base = static_cast<store_base *>(this);

        if constexpr (std::is_same_v<decltype(base->t), Target>) { return dttl_store<true, Target &>{base->t}; }

        return dttl_store<false, Target &>{};
    }

    template <class... OtherCells>
    using add = dttl<StoreValues,
        Length + sizeof...(OtherCells) + 1, OtherCells...,
        Last<Dependent...>>;
};

template <bool StoreValues, size_t Length, template <class...> class Head, class... HeadDependent, class... Tail>
struct dttl<StoreValues, Length, Head<HeadDependent...>, Tail...> : dttl_store<StoreValues, Head<HeadDependent...>>,
                                                                    dttl<StoreValues, Length, Tail...> {
private:
    using base = dttl_store<StoreValues, Head<HeadDependent...>>;
    using store_base = dttl_store<StoreValues, Head<HeadDependent...>>;

public:
    template <class Target>
    constexpr static bool has = std::is_same_v<Target, Head> ? true : base::template has<Target>;

    template <size_t Index, typename = std::enable_if_t<Index == Length - sizeof...(Tail) - 1>>
    using get_type = Head<HeadDependent...>;

    template <size_t Index, typename = std::enable_if_t<(Index == Length - sizeof...(Tail) - 1) && StoreValues>>
    constexpr auto &get_value() {
        return static_cast<store_base *>(this)->t;
    }

    template <class Target, bool Found, typename = std::enable_if_t<StoreValues>>
    constexpr dttl_store<Found, Target &> find() {
        auto *base = static_cast<store_base *>(this);

        if constexpr (std::is_same_v<decltype(base->t), Target>) { return dttl_store<true, Target &>{base->t}; }

        return dttl_store<false, Target &>{};
    }

    template <class... OtherCells>
    using add = dttl<StoreValues,
        Length + sizeof...(Tail) + sizeof...(OtherCells),
        OtherCells..., Head<HeadDependent...>, Tail...>;
};

}
