#pragma once

#include "sdl.hpp"

#include <sol.hpp>

#include <chrono>

struct SDL_MouseButtonEvent;

namespace ember {

class engine;

class scene {
public:
    scene(engine& eng) : engine(&eng) {}
    virtual ~scene() = 0;

    virtual void init() = 0;
    virtual void tick(float delta) = 0;
    virtual void render() = 0;
    virtual auto handle_game_input(const SDL_Event& event) -> bool = 0;
    virtual auto render_gui() -> sol::table = 0;

    engine* engine;
};

inline scene::~scene() = default;

} // namespace ember
