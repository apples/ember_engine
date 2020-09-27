#include "box2d_helpers.hpp"

namespace ember {

box2d_fixed_timestep::box2d_fixed_timestep() : box2d_fixed_timestep(1.f / 60.f) {}

box2d_fixed_timestep::box2d_fixed_timestep(float timeStep) : box2d_fixed_timestep(timeStep, 10, 8) {}

box2d_fixed_timestep::box2d_fixed_timestep(float timeStep, int velocityIterations, int positionIterations)
    : timeStep(timeStep), velocityIterations(velocityIterations), positionIterations(positionIterations), accum(0) {}

auto box2d_fixed_timestep::step(b2World& world, float delta) -> bool {
    accum += delta;
    if (accum >= timeStep) {
        accum -= timeStep;
        world.Step(timeStep, velocityIterations, positionIterations);
        return true;
    }
    return false;
}

} // namespace ember
