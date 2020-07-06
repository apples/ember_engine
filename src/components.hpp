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

#define EMBER_REFLECTION_ENABLE_REGISTRY
#include "ember/reflection_start.hpp"

namespace component {

/** Actor script used for various events */
struct script {
    std::string name; /** Script name within the 'actors.' namespace */
    int next_tick; /** Ticks until next update, if -1 then never updates */
};
REFLECT(script, (name)(next_tick))

/** World transform */
struct transform : sushi::transform {};
REFLECT(transform, (pos)(rot)(scl))

struct sprite {
    std::string texture;
    glm::vec2 size = {1, 1};
    std::vector<int> frames;
    float time = 0;
};
REFLECT(sprite, (texture)(size)(frames)(time))

struct body {
    enum type_t {
        BALL,
        PADDLE,
        BRICK,
        N_TYPES,
    };

    glm::vec2 size = {0, 0};
    glm::vec2 vel = {0, 0};
    type_t type;
    std::array<bool, N_TYPES> collides_with = {};
};
REFLECT(body, (size)(vel)(type)(collides_with))

} // namespace component

#include "ember/reflection_end.hpp"
