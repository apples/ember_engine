#include "engine.hpp"

#include <iostream>

namespace ember {

void engine::tick() try {
    using namespace std::literals;

    // Update clock

    auto now = clock::now();
    auto delta = now - prev_time;
    prev_time = now;

    framerate_buffer.push_back(delta);

    if (framerate_buffer.size() >= 10) {
        auto avg_frame_dur = std::accumulate(begin(framerate_buffer), end(framerate_buffer), 0ns) / framerate_buffer.size();
        auto framerate = 1.0 / std::chrono::duration<double>(avg_frame_dur).count();

        gui_state["fps"] = std::to_string(std::lround(framerate)) + "fps";

        framerate_buffer.clear();
    }

    // Pump events

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (handle_gui_input(event))
            continue;
        if (handle_game_input(event))
            continue;
    }

    // Run current scene

    if (current_scene) {
        current_scene->tick(std::chrono::duration<float>(delta).count());
    }

    // Render scene

    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (current_scene) {
        current_scene->render();
        update_gui_state(current_scene->render_gui());
    }

    // Render GUI

    gui::calculate_all_layouts(*root_widget);

    renderer.begin();
    gui::draw_all(*root_widget);
    renderer.end();

    // End of frame

    if (queued_transition) {
        current_scene = queued_transition->factory(*this, current_scene.get());
        lua["scene"] = current_scene;
        queued_transition = std::nullopt;
        current_scene->init();
    }

    lua.collect_garbage();

    SDL_GL_SwapWindow(window);
} catch (const std::exception& e) {
    std::cerr << "ember::engine::tick: EXCEPTION: " << e.what() << std::endl;
    std::throw_with_nested(std::runtime_error("tick: "));
}

bool engine::get_should_quit() const {
    return should_quit;
}

auto engine::handle_game_input(const SDL_Event& event) -> bool try {
    switch (event.type) {
    case SDL_QUIT:
        std::cout << "Goodbye!" << std::endl;
        should_quit = true;
        return true;
    }

    if (current_scene) {
        return current_scene->handle_game_input(event);
    } else {
        return false;
    }
} catch (...) {
    std::throw_with_nested(std::runtime_error("handle_game_input: "));
}

auto engine::handle_gui_input(SDL_Event& e) -> bool try {
    switch (e.type) {
    case SDL_TEXTINPUT: {
        if (auto widget = focused_widget.lock()) {
            if (widget->on_textinput) {
                widget->on_textinput(widget, e.text.text);
                return true;
            }
        }
        break;
    }
    case SDL_KEYDOWN: {
        if (auto widget = focused_widget.lock()) {
            if (widget->on_keydown) {
                return widget->on_keydown(widget, SDL_GetKeyName(e.key.keysym.sym));
            }
        }
    }
    case SDL_MOUSEBUTTONDOWN: {
        switch (e.button.button) {
        case SDL_BUTTON_LEFT: {
            auto abs_click_pos = glm::vec2{e.button.x, display.height - e.button.y + 1};
            auto widget_stack = get_descendent_stack(*root_widget, abs_click_pos);
            while (!widget_stack.empty()) {
                auto cur_widget = widget_stack.back();
                auto widget_pos = cur_widget->get_layout().position;
                auto rel_click_pos = abs_click_pos - widget_pos;
                if (cur_widget->on_click) {
                    cur_widget->on_click(cur_widget->shared_from_this(), rel_click_pos);
                    return true;
                } else if (cur_widget->pointer_opaque()) {
                    return true;
                } else {
                    widget_stack.pop_back();
                }
            }
            break;
        }
        }
        break;
    }
    }
    return false;
} catch (...) {
    std::throw_with_nested(std::runtime_error("handle_gui_input: "));
}

} // namespace ember
