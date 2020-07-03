#include "lua_system.hpp"

namespace ember {

void run_lua_system(sol::state& lua, const std::string& name, float delta, ember::database& entities) {
    auto module_name = "systems." + name;
    auto file_name = "data/scripts/systems/" + name + ".lua";
    auto scripting = sol::table(lua.require_file(module_name, file_name));
    sol::protected_function visit = scripting["visit"];
    auto visit_result = visit(delta, entities);

    if (!visit_result.valid()) {
        sol::error err = visit_result;
        throw std::runtime_error(std::string("System failure: " + name + ": ") + err.what());
    }
}

} // namespace ember
