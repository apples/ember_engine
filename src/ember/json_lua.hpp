#pragma once

#include "json.hpp"

#include <sol.hpp>

#include <string>

namespace ember::json_lua {

auto json_to_lua(const nlohmann::json& json, sol::this_state state) -> sol::object;

auto lua_to_json(const sol::object& obj) -> nlohmann::json;

auto load_file(const std::string& filename, sol::this_state state) -> sol::object;

} // namespace ember::json_lua
