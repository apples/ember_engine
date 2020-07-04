#include "lua_gui.hpp"

#include "gui.hpp"

namespace ember::lua_gui {

void register_types(sol::table& lua) {
    using gui::block_layout;
    using gui::widget;

    auto block_layout_type = lua.new_usertype<block_layout>("block_layout", sol::constructors<>{});
    block_layout_type["position"] = &block_layout::position;
    block_layout_type["size"] = &block_layout::size;
    block_layout_type["visible"] = &block_layout::visible;

    auto widget_type = lua.new_usertype<widget>("widget", sol::constructors<>{});
    widget_type["get_type"] = &widget::get_type;
    widget_type["create_widget"] = &widget::create_widget;
    widget_type["get_parent"] = &widget::get_parent;
    widget_type["get_children"] = &widget::get_children;
    widget_type["add_child"] = &widget::add_child;
    widget_type["remove_child"] = &widget::remove_child;
    widget_type["replace_child"] = &widget::replace_child;
    widget_type["clear_children"] = &widget::clear_children;
    widget_type["set_attribute"] = &widget::set_attribute;
    widget_type["get_attribute"] = &widget::get_attribute;
    widget_type["get_all_attributes"] = &widget::get_all_attributes;
    widget_type["on_click"] = &widget::on_click;
    widget_type["on_textinput"] = &widget::on_textinput;
    widget_type["on_keydown"] = &widget::on_keydown;
    widget_type["get_layout"] = &widget::get_layout;
}

} // namespace ember::lua_gui
