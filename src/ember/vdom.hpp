#pragma once

#include <sol.hpp>

#include <algorithm>
#include <string>

namespace ember::vdom {

auto create_element(
    sol::state& lua, const std::string& module_name, const sol::table& props, const sol::table& children) -> sol::table;

} // namespace ember::vdom
