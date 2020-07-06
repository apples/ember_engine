#pragma once

#include "scripting.hpp"
#include "utility.hpp"
#include "reflection.hpp"
#include "net_id.hpp"

#include <ginseng/ginseng.hpp>

#include <cstdint>
#include <unordered_map>
#include <optional>
#include <tuple>

namespace ember {

class database : public ginseng::database {
    template <typename... Coms>
    struct entity_serializer {
        static auto serialize(database& db, ent_id eid) -> std::tuple<std::optional<Coms>...> {
            return {};
        }
    };

    template <typename Head, typename... Coms>
    struct entity_serializer<Head, Coms...> {
        static auto serialize(database& db, ent_id eid) -> std::tuple<std::optional<Head>, std::optional<Coms>...> {
            auto tup = entity_serializer<Coms...>::serialize(db, eid);
            auto com = std::optional<Head>{};
            if (db.has_component<Head>(eid)) {
                com = db.get_component<Head>(eid);
            }
            return std::tuple_cat(std::tuple{std::move(com)}, std::move(tup));
        }
    };

public:
    auto create_entity() -> ent_id;

    auto create_entity(net_id::id_type id) -> ent_id;

    void destroy_entity(net_id::id_type id);

    void destroy_entity(ent_id eid);

    auto get_entity(net_id::id_type id) -> std::optional<ent_id>;

    auto get_or_create_entity(net_id::id_type id) -> ent_id;

    void on_destroy_entity(std::function<void(net_id::id_type id)> func);

    template <typename... Coms>
    auto serialize_entity(ent_id eid) -> std::tuple<std::optional<Coms>...> {
        return entity_serializer<Coms...>::serialize(*this, eid);
    }

    template <typename T>
    com_id add_component(ent_id eid, T&& com) {
        using com_type = std::decay_t<T>;
        auto guid = ginseng::_detail::get_type_guid<com_type>();

        ++com_counts[guid];

        return ginseng::database::add_component(eid, std::forward<T>(com));
    }

    template <typename T>
    void add_component(ent_id eid, ginseng::tag<T> com) {
        auto guid = ginseng::_detail::get_type_guid<ginseng::tag<T>>();

        ++com_counts[guid];

        return ginseng::database::add_component(eid, com);
    }

    template <typename Com>
    void remove_component(ent_id eid) {
        auto guid = ginseng::_detail::get_type_guid<Com>();

        --com_counts[guid];

        ginseng::database::remove_component<Com>(eid);
    }

    template <typename Com>
    int count_components() const {
        auto guid = ginseng::_detail::get_type_guid<Com>();

        auto iter = com_counts.find(guid);

        if (iter != com_counts.end()) {
            return iter->second;
        } else {
            return 0;
        }
    }

private:
    net_id::id_type next_id = 1;
    std::unordered_map<net_id::id_type, ent_id> netid_to_entid;
    std::function<void(net_id::id_type id)> destroy_entity_callback;
    std::unordered_map<ginseng::_detail::type_guid, int> com_counts;
};

namespace scripting {

template <>
void register_type<database>(sol::table& lua);

} //namespace scripting

} // namespace ember

namespace sol {

template <>
struct is_automagical<ember::database> : std::false_type {};

} //namespace sol
