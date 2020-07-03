
#include "deserialize_component.hpp"
#include "lua_gui.hpp"
#include "gui.hpp"
#include "state.hpp"
#include "state_gameplay.hpp"
#include "state_login.hpp"
#include "state_mainmenu.hpp"

#include "emberjs/websocket.hpp"
#include "emberjs/config.hpp"

#include <common/cereal.hpp>

#include <glm/gtx/intersect.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/noise.hpp>

#include <cmath>
#include <memory>
#include <random>

void ember_client_engine::tick() try {
    using namespace std::literals;

    auto now = clock::now();
    auto delta_time = now - prev_time;
    prev_time = now;

    const auto delta = std::chrono::duration<double>(delta_time).count();

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (handle_gui_input(event)) break;
        if (handle_game_input(event)) break;
    }

    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    current_state->tick(delta_time, delta, keys);

    framerate_buffer.push_back(delta_time);

    if (framerate_buffer.size() >= 10) {
        auto avg_frame_dur = std::accumulate(begin(framerate_buffer), end(framerate_buffer), 0ns) / framerate_buffer.size();
        auto framerate = 1.0 / std::chrono::duration<double>(avg_frame_dur).count();

        gui_state["fps"] = std::to_string(std::lround(framerate)) + "fps";

        framerate_buffer.clear();
    }

    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    current_state->render(program_basic);

    update_gui_state(current_state->render_gui());
    gui::calculate_all_layouts(*root_widget);

    renderer.begin();
    gui::draw_all(*root_widget);
    renderer.end();

    lua.collect_garbage();

    SDL_GL_SwapWindow(g_window);
} catch (const std::exception& e) {
    std::cerr << "tick exception: " << e.what() << std::endl;
    std::throw_with_nested(std::runtime_error("tick: "));
}
