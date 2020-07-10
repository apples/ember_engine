#pragma once

#include "ember/engine.hpp"
#include "ember/entities.hpp"

class physics_system {
public:
    void step(ember::engine& engine, ember::database& entities, float delta);
private:
    float time = 0;
};
