#pragma once

#include "entities.hpp"

#include <sol.hpp>

#include <string>

namespace ember {

void run_lua_system(sol::state& lua, const std::string& name, float delta, ember::database& entities);

} // namespace ember
