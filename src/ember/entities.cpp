#include "entities.hpp"

#include "components.hpp"

#include <iostream>

namespace ember {

auto database::create_entity() -> ent_id {
    return create_entity(next_id++);
}

auto database::create_entity(net_id id) -> ent_id {
    auto iter = netid_to_entid.find(id);

    if (iter != netid_to_entid.end()) {
        std::clog << "Warning: Entity " << id << " created twice!" << std::endl;
        return iter->second;
    }

    auto ent = database::create_entity();
    database::add_component(ent, component::net_id{id});
    netid_to_entid[id] = ent;

    return ent;
}

void database::destroy_entity(net_id id) {
    auto iter = netid_to_entid.find(id);

    if (iter == netid_to_entid.end()) {
        std::clog << "Warning: Attempted to erase unknown entity " << id << "!" << std::endl;
        return;
    }

    if (destroy_entity_callback) {
        destroy_entity_callback(id);
    }

    database::destroy_entity(iter->second);
    netid_to_entid.erase(iter);
}

void database::destroy_entity(ent_id eid) {
    if (has_component<component::net_id>(eid)) {
        auto id = get_component<component::net_id>(eid).id;

        if (destroy_entity_callback) {
            destroy_entity_callback(id);
        }

        auto iter = netid_to_entid.find(id);
        netid_to_entid.erase(iter);
    } else {
        std::clog << "Warning: deleting entity that has no net_id!" << std::endl;
    }

    database::destroy_entity(eid);
}

auto database::get_entity(net_id id) -> std::optional<ent_id> {
    auto iter = netid_to_entid.find(id);
    if (iter == netid_to_entid.end()) {
        return std::nullopt;
    } else {
        return iter->second;
    }
}

auto database::get_or_create_entity(net_id id) -> ent_id {
    auto iter = netid_to_entid.find(id);
    if (iter == netid_to_entid.end()) {
        return create_entity(id);
    } else {
        return iter->second;
    }
}

void database::on_destroy_entity(std::function<void(net_id id)> func) {
    destroy_entity_callback = std::move(func);
}

namespace scripting {

template <>
void register_type<database>(sol::table& lua) {
    lua.new_usertype<database::ent_id>("ent_id", sol::constructors<>(),
        "get_index", &database::ent_id::get_index,
        sol::meta_function::equal_to, &database::ent_id::operator==);

    lua.new_usertype<database>("database",
        "create_entity", sol::overload(
            sol::resolve<database::ent_id()>(&database::create_entity),
            sol::resolve<database::ent_id(database::net_id)>(&database::create_entity)),
        "destroy_entity", sol::overload(
            sol::resolve<void(database::net_id)>(&database::destroy_entity),
            sol::resolve<void(database::ent_id)>(&database::destroy_entity)),
        "get_entity", [](database& db, database::net_id id, sol::this_state s) -> sol::object {
            auto lua = sol::state_view(s);
            auto eid = db.get_entity(id);
            if (eid) {
                return sol::make_object(lua, *eid);
            } else {
                return sol::make_object(lua, sol::nil);
            }
        },
        "get_or_create_entity", &database::get_or_create_entity,
        "add_component", [](database& db, database::ent_id eid, sol::userdata com){
            return com["_add_component"](db, eid, com);
        },
        "remove_component", [](database& db, database::ent_id eid, sol::table com_type){
            return com_type["_remove_component"](db, eid);
        },
        "get_component", [](database& db, std::optional<database::ent_id> eid, sol::table com_type){
            if (eid) {
                return com_type["_get_component"](db, eid);
            } else {
                throw std::runtime_error("get_component: Null eid!");
            }
        },
        "has_component", [](database& db, database::ent_id eid, sol::table com_type){
            return com_type["_has_component"](db, eid);
        },
        "visit", [](database& db, sol::protected_function func){
            db.visit([&func](database::ent_id eid) {
                auto result = func(eid);
                if (!result.valid()) {
                    sol::error error = result;
                    throw std::runtime_error(std::string("database.visit(): ") + error.what());
                }
            });
        });
}

} //namespace scripting

} // namespace ember
