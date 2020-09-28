#include "entities.hpp"

#include "net_id.hpp"

#include <iostream>

namespace ember {

auto database::create_entity() -> ent_id {
    return create_entity(next_id++);
}

auto database::create_entity(net_id::id_type id) -> ent_id {
    auto iter = netid_to_entid.find(id);

    if (iter != netid_to_entid.end()) {
        std::clog << "Warning: Entity " << id << " created twice!" << std::endl;
        return iter->second;
    }

    auto ent = ginseng::database::create_entity();
    ginseng::database::add_component(ent, net_id{id});
    netid_to_entid.insert_or_assign(id, ent);

    return ent;
}

void database::destroy_entity(net_id::id_type id) {
    auto iter = netid_to_entid.find(id);

    if (iter == netid_to_entid.end()) {
        std::clog << "Warning: Attempted to erase unknown entity " << id << "!" << std::endl;
        return;
    }

    if (destroy_entity_callback) {
        destroy_entity_callback(id);
    }

    ginseng::database::destroy_entity(iter->second);
    netid_to_entid.erase(iter);
}

void database::destroy_entity(ent_id eid) {
    if (has_component<net_id>(eid)) {
        auto id = get_component<net_id>(eid).id;

        if (destroy_entity_callback) {
            destroy_entity_callback(id);
        }

        auto iter = netid_to_entid.find(id);
        netid_to_entid.erase(iter);
    } else {
        std::clog << "Warning: deleting entity that has no net_id!" << std::endl;
    }

    ginseng::database::destroy_entity(eid);
}

auto database::get_entity(net_id::id_type id) -> std::optional<ent_id> {
    auto iter = netid_to_entid.find(id);
    if (iter == netid_to_entid.end()) {
        return std::nullopt;
    } else {
        return iter->second;
    }
}

auto database::get_or_create_entity(net_id::id_type id) -> ent_id {
    auto iter = netid_to_entid.find(id);
    if (iter == netid_to_entid.end()) {
        return create_entity(id);
    } else {
        return iter->second;
    }
}

void database::on_destroy_entity(std::function<void(net_id::id_type id)> func) {
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
            sol::resolve<database::ent_id(net_id::id_type)>(&database::create_entity)),
        "destroy_entity", sol::overload(
            sol::resolve<void(net_id::id_type)>(&database::destroy_entity),
            sol::resolve<void(database::ent_id)>(&database::destroy_entity)),
        "exists", &database::exists,
        "get_entity", [](database& db, net_id::id_type id, sol::this_state s) -> sol::object {
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
            auto add_component = com["_add_component"];
            if (!add_component.valid()) {
                throw std::runtime_error("add_component: Component type missing _add_component");
            }
            return add_component(db, eid, com);
        },
        "remove_component", [](database& db, database::ent_id eid, sol::table com_type){
            auto remove_component = com_type["_remove_component"];
            if (!remove_component.valid()) {
                throw std::runtime_error("remove_component: Component type missing _remove_component");
            }
            return remove_component(db, eid);
        },
        "get_component", [](database& db, std::optional<database::ent_id> eid, sol::table com_type){
            if (eid) {
                auto get_component = com_type["_get_component"];
                if (!get_component.valid()) {
                    throw std::runtime_error("get_component: Component type missing _get_component");
                }
                return get_component(db, eid);
            } else {
                throw std::runtime_error("get_component: Null eid!");
            }
        },
        "has_component", [](database& db, database::ent_id eid, sol::table com_type){
            auto has_component = com_type["_has_component"];
            if (!has_component.valid()) {
                throw std::runtime_error("has_component: Component type missing _has_component");
            }
            return has_component(db, eid);
        },
        "visit", [](database& db, sol::protected_function func){
            db.visit([&func](database::ent_id eid) {
                auto result = func(eid);
                if (!result.valid()) {
                    sol::error error = result;
                    throw std::runtime_error(std::string("database.visit(): ") + error.what());
                }
            });
        },
        "to_ptr", &database::to_ptr,
        "from_ptr", &database::from_ptr);
}

} //namespace scripting

} // namespace ember
