#pragma once

#include "ember/entities.hpp"

class physics_system {
public:
    void step(ember::database& entities, float delta);
private:
    float time = 0;
};
