#pragma once

#include "json.hpp"

#include "reflection.hpp"

#include <type_traits>

namespace ember::json_serializers::basic {

template <typename T>
auto from_json(const nlohmann::json& json, T& msg) -> std::enable_if_t<reflection::refl_traits<T>::is_reflectable> {
    using type = std::decay_t<T>;
    auto refl = reflect<type>();
    std::apply([&](auto&&... members) { ((msg.*members.ptr() = json[members.name()]), ...); }, refl.members);
}

template <typename T>
auto to_json(nlohmann::json& json, const T& msg) -> std::enable_if_t<reflection::refl_traits<T>::is_reflectable> {
    using type = std::decay_t<T>;
    auto refl = reflect<type>();
    std::apply([&](auto&&... members) { ((json[members.name()] = msg.*members.ptr()), ...); }, refl.members);
}

} // namespace ember::json_serializers::basic

namespace ember::json_serializers::with_type {

template <typename T>
auto from_json(const nlohmann::json& json, T& msg) -> std::enable_if_t<reflection::refl_traits<T>::is_reflectable> {
    using type = std::decay_t<T>;
    auto refl = reflect<type>();
    std::apply([&](auto&&... members) { ((msg.*members.ptr() = json[members.name()]), ...); }, refl.members);
}

template <typename T>
auto to_json(nlohmann::json& json, const T& msg) -> std::enable_if_t<reflection::refl_traits<T>::is_reflectable> {
    using type = std::decay_t<T>;
    auto refl = reflect<type>();
    json["type"] = refl.name;
    std::apply([&](auto&&... members) { ((json[members.name()] = msg.*members.ptr()), ...); }, refl.members);
}

} // namespace ember::json_serializers::with_type
