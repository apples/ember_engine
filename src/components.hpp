#pragma once

#include "ember/component_common.hpp"
#include "ember/entities.hpp"
#include "ember/net_id.hpp"
#include "ember/ez3d.hpp"

#include <sushi/sushi.hpp>

#include <chrono>
#include <functional>
#include <optional>
#include <string>
#include <memory>

#define EMBER_REFLECTION_ENABLE_REGISRTY
#include "ember/reflection_start.hpp"

namespace component {

using ember::net_id;
REFLECT(net_id, (id))

/** World transform */
struct transform : sushi::transform {};
REFLECT(transform, (pos)(rot)(scl))

/** Model used for rendering */
struct render_model : ember::ez3d::model {};
REFLECT(render_model, (mesh)(texture)(skeleton)(anim_index)(anim_time))

/** Actor script used for various events */
struct script {
    std::string name; /** Script name within the 'actors.' namespace */
    int next_tick; /** Ticks until next update, if -1 then never updates */
};
REFLECT(script, (name)(next_tick))

} // namespace component

#include "ember/reflection_end.hpp"
