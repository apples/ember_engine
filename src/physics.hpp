#pragma once

#include "ember/engine.hpp"
#include "ember/entities.hpp"

class physics_system {
public:
    struct body {
        enum type_t {
            STATIC,
            KINEMATIC,
            DYNAMIC
        };

        enum layer_t {
            BALL,
            PADDLE,
            BRICK,
            N_TYPES,
        };

        type_t type;
        layer_t layer;
        std::array<bool, N_TYPES> collides_with = {};
        glm::vec2 size = {0, 0};
        glm::vec2 vel = {0, 0};
    };

    void step(ember::engine& engine, ember::database& entities, float delta);
private:
    float time = 0;
};
