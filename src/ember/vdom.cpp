#include "vdom.hpp"

namespace ember::vdom {

auto create_element(
    sol::state& lua, const std::string& module_name, const sol::table& props, const sol::table& children)
    -> sol::table {
    auto module_path = module_name;
    std::replace(begin(module_path), end(module_path), '.', '/');
    auto file_name = "data/scripts/" + module_path + ".lua";
    auto component = lua.require_file(module_name, file_name);
    auto element = lua.create_table();
    element["type"] = component;
    element["props"] = props;
    element["children"] = children;
    return element;
}

} // namespace ember::vdom
