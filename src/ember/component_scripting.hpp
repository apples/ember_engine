#pragma once

#include "scripting.hpp"
#include "entities.hpp"
#include "reflection.hpp"

#include <type_traits>
#include <utility>
#include <iostream>
#include <stdexcept>

#define EMBER_REGISTER_ALL_COMPONENTS                                                                              \
    using registered_types =                                                                                       \
        decltype(_type_registry(std::declval<_type_registry_index<EMBER_REFLECTION_MAX_TYPES>>()));                \
    using ember::component::register_usertype;                                                                     \
    void register_all_components(sol::table& table) {                                                              \
        std::apply(                                                                                                \
            [&](auto... types) { (ember::scripting::register_type<typename decltype(types)::type>(table), ...); }, \
            registered_types{});                                                                                   \
    }

namespace ember::component {
namespace _detail {

using ent_id = database::ent_id;
using com_id = database::com_id;

template <typename T>
struct is_tag : std::false_type {};

template <typename T>
struct is_tag<ginseng::tag<T>> : std::true_type {};

template <typename T>
void add_component(database& db, ent_id eid, T com) {
    db.add_component(eid, std::move(com));
}

template <typename T>
void remove_component(database& db, ent_id eid) {
    if (!db.has_component<T>(eid)) {
        auto name = std::string(reflect<T>().name);
        auto id = std::to_string(eid.get_index());

        throw std::runtime_error("Bad component removal: " + name + " from entity " + id);
    }

    db.remove_component<T>(eid);
}

template <typename T>
auto get_component(database& db, ent_id eid) -> T* {
    if (!db.has_component<T>(eid)) {
        auto name = std::string(reflect<T>().name);
        auto id = std::to_string(eid.get_index());

        throw std::runtime_error("Bad component access: " + name + " from entity " + id);
    }

    return db.get_component<T*>(eid);
}

template <typename T>
auto has_component(database& db, database::ent_id eid) -> bool {
    return db.has_component<T>(eid);
}

} //namespace _detail

template <typename T>
void register_usertype(scripting::token<T>, sol::usertype<T>& usertype) {
    using namespace _detail;

    if constexpr (is_tag<T>::value) {
        usertype["new"] = sol::constructors<T()>{};
    } else {
        usertype["new"] = sol::constructors<T(), T(const T&)>{};
        usertype["_get_component"] = &_detail::get_component<T>;
    }

    usertype["_add_component"] = &_detail::add_component<T>;
    usertype["_remove_component"] = &_detail::remove_component<T>;
    usertype["_has_component"] = &_detail::has_component<T>;
}

} // namespace ember::component
