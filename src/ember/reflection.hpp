#pragma once

#include <tuple>

namespace ember::reflection {

/** Type tag used to get ADL for reflected types */
template <typename T>
struct tag {
    using type = T;
};

/** Type member reflection info */
template <auto P>
struct member {
    constexpr explicit member(const char* n) : _name(n) {}
    /** Name of the member */
    constexpr auto name() const { return _name; }
    /** Pointer to the member */
    constexpr auto ptr() const { return P; }
private:
    const char* _name;
};

/** Type reflection info */
template <typename T, auto... Ps>
struct refl_info {
    /** Type */
    using type = T;
    /** Name of the type */
    const char* name;
    /** Members of the type */
    std::tuple<member<Ps>...> members;
};

/** Crates a refl_info */
template <typename T, auto... Ps>
constexpr auto make_refl_info(tag<T>, const char* name, std::tuple<member<Ps>...> members) {
    return refl_info<T, Ps...>{name, members};
}

/** Get reflection info for a type */
template <typename T>
constexpr auto reflect() {
    return reflect(tag<T>{});
}

/** Reflection traits, determines if a type is reflectable or not. */
template <typename T, typename = void>
struct refl_traits {
    static constexpr bool is_reflectable = false;
};

template <typename T>
struct refl_traits<T, std::void_t<decltype(reflect(std::declval<tag<T>>()))>> {
    static constexpr bool is_reflectable = true;
};

} // namespace ember::reflection

namespace ember {

using reflection::reflect;

} // namespace ember
