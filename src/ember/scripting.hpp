#pragma once

#include "reflection.hpp"

#include <sol.hpp>

namespace ember::scripting {

template <typename T>
struct token {};

template <typename T, typename = void>
struct has_register : std::false_type {};

template <typename T>
struct has_register<T, std::void_t<decltype(register_usertype(token<T>{}, std::declval<sol::usertype<T>&>()))>>
    : std::true_type {};

/** Registers the type T with Lua, calling register_usertype(token<T>) if available. */
template <typename T>
void register_type(sol::table& lua) {
    auto usertype = lua.new_usertype<T>(reflect<T>().name);

    std::apply(
        [&](auto&&... members) {
            if constexpr (has_register<T>::value) {
                register_usertype(token<T>{}, usertype);
            } else {
                usertype["new"] = sol::constructors<T(), T(const T&)>{};
            }

            ((usertype[members.name()] = members.ptr()), ...);
        },
        reflect<T>().members);
}

} // namespace ember::scripting
