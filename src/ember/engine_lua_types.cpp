#include "engine.hpp"

#include "math.hpp"

namespace ember {

void engine::register_engine_module() {
    auto global_table = sol::table(lua.globals());
    scripting::register_type<database>(global_table);

    auto engine_table = lua.create_table();
    engine_table["entities"] = std::ref(entities);
    engine_table["hub_service"] = std::ref(hub_service);

    lua["package"]["loaded"]["engine"] = engine_table;
}

} // namespace ember
