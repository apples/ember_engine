#pragma once

#include "config.hpp"
#include "display.hpp"
#include "font.hpp"
#include "resource_cache.hpp"
#include "sdl.hpp"
#include "sushi_renderer.hpp"
#include "state.hpp"

#include <sol.hpp>
#include <sushi/sushi.hpp>

#include <chrono>

namespace ember {

class engine {
public:
    using clock = std::chrono::steady_clock;
    using delta_duration = std::chrono::nanoseconds;

    engine(const config::config& config);
    engine(const engine&) = delete;
    engine(engine&&) = delete;
    engine& operator=(const engine&) = delete;
    engine& operator=(engine&&) = delete;
    ~engine();

    auto handle_game_input(const SDL_Event& event) -> bool;
    auto handle_gui_input(SDL_Event& e) -> bool;

    void tick();

    sol::state lua;
    display_info display;
    resource_cache<sushi::mesh_group, std::string> mesh_cache;
    resource_cache<sushi::skeleton, std::string> skeleton_cache;
    resource_cache<sushi::texture_2d, std::string> texture_cache;
    resource_cache<msdf_font, std::string> font_cache;

private:
    void register_engine_module();

    SDL_Window* window;
    SDL_GLContext glcontext;

    clock::time_point prev_time;
    std::vector<delta_duration> framerate_buffer;

    sushi_renderer renderer;
    std::shared_ptr<gui::widget> root_widget;
    std::weak_ptr<gui::widget> focused_widget;
    sol::table gui_state;
    sol::function update_gui_state;

    std::unique_ptr<state> current_state;
};

} // namespace ember
