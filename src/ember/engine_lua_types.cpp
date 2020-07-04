#include "engine.hpp"

#include "math.hpp"
#include "scripting.hpp"

#include <sol.hpp>

namespace ember {

void engine::register_engine_module() {
    auto engine_table = lua.create_table();
    lua["package"]["loaded"]["engine"] = engine_table;
}

} // namespace ember
