#include "components.hpp"

#include "ember/scripting.hpp"

namespace component {

using registered_types = decltype(_type_registry(std::declval<_type_registry_index<EMBER_COMPONENT_MAX_TYPES>>()));

void register_all_components(sol::table& table) {
    using ember::scripting::register_type;

    std::apply(
        [&](auto&&... types) { (register_type<std::decay_t<decltype(types)>>(table), ...); }, registered_types{});
}

} // namespace component
