#pragma once

#include <sol_forward.hpp>

#include <box2d/box2d.h>

namespace ember {

void box2d_lua_enable(sol::state& tab);

class box2d_fixed_timestep {
public:
    box2d_fixed_timestep();

    explicit box2d_fixed_timestep(float timeStep);

    box2d_fixed_timestep(float timeStep, int velocityIterations, int positionIterations);

    auto step(b2World& world, float delta) -> bool;

private:
    float timeStep;
    int velocityIterations;
    int positionIterations;
    float accum;
};

} // namespace ember
