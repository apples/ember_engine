#pragma once

#include "sdl.hpp"
#include "shaders.hpp"

#include <sol.hpp>

#include <chrono>

struct SDL_MouseButtonEvent;

namespace ember {

class state {
public:
    virtual ~state() = 0;

    virtual void tick(std::chrono::nanoseconds delta) = 0;
    virtual void render(basic_shader_program& program_basic) = 0;
    virtual auto handle_game_input(const SDL_Event& event) -> bool = 0;
    virtual auto render_gui() -> sol::table = 0;
};

inline state::~state() = default;

} // namespace ember
