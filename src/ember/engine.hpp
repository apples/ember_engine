#pragma once

#include "config.hpp"
#include "display.hpp"
#include "font.hpp"
#include "resource_cache.hpp"
#include "sdl.hpp"
#include "sushi_renderer.hpp"
#include "scene.hpp"
#include "shaders.hpp"

#include <sol.hpp>
#include <sushi/sushi.hpp>

#include <chrono>
#include <sstream>
#include <iostream>

namespace ember {

class scene;

class engine {
public:
    using clock = std::chrono::steady_clock;

    engine(const config::config& config);
    engine(const engine&) = delete;
    engine(engine&&) = delete;
    engine& operator=(const engine&) = delete;
    engine& operator=(engine&&) = delete;
    ~engine();

    auto handle_game_input(const SDL_Event& event) -> bool;
    auto handle_gui_input(SDL_Event& e) -> bool;

    void tick();

    bool get_should_quit() const;

    template <typename... Ts>
    auto call_script(const std::string& module_name, const std::string& function_name, Ts&&... args)
        -> sol::function_result;

    template <typename T, typename = std::enable_if_t<std::is_base_of_v<scene, T>>>
    void queue_transition(bool force = false);

    sol::state lua;
    display_info display;
    resource_cache<sushi::mesh_group, std::string> mesh_cache;
    resource_cache<sushi::skeleton, std::string> skeleton_cache;
    resource_cache<sushi::texture_2d, std::string> texture_cache;
    resource_cache<msdf_font, std::string> font_cache;
    shaders::basic_shader_program basic_shader;
    shaders::msdf_shader_program msdf_shader;

private:
    void register_engine_module();

    SDL_Window* window;
    SDL_GLContext glcontext;

    bool should_quit;

    clock::time_point prev_time;
    std::vector<clock::duration> framerate_buffer;

    sushi_renderer renderer;
    std::shared_ptr<gui::widget> root_widget;
    std::weak_ptr<gui::widget> focused_widget;
    sol::table gui_state;
    sol::function update_gui_state;

    std::shared_ptr<scene> current_scene;

    struct transition {
        std::function<std::shared_ptr<scene>(engine& eng, scene* previous)> factory;
        bool force;
    };

    std::optional<transition> queued_transition;
};

template <typename... Ts>
auto engine::call_script(const std::string& module_name, const std::string& function_name, Ts&&... args)
    -> sol::function_result {
    lua["err_handler"] = [&](const std::string& err) {
        lua.script("trace = debug.traceback()");
        lua.script("print(debug.traceback())");
        std::cout << "err_handler: " << err << std::endl;
        return err + lua["trace"].get<std::string>();
    };
    auto module_path = module_name;
    std::replace(begin(module_path), end(module_path), '.', '/');
    auto file_name = "data/scripts/" + module_path + ".lua";
    auto module = sol::table(lua.require_file(module_name, file_name));
    auto func = sol::protected_function(module[function_name], lua["err_handler"]);
    auto result = func(std::forward<Ts>(args)...);
    if (result.valid()) {
        return result;
    } else {
        auto err = sol::error(result);
        auto oss = std::ostringstream{};
        oss << "ERROR: engine::call_script(\"" << module_name << "\", \"" << function_name << "\", ...): " << err.what()
            << "(status = " << int(result.status()) << ")"
            << "\n";
        std::cerr << oss.str();
        throw std::runtime_error(oss.str());
    }
}

template <typename T, typename>
void engine::queue_transition(bool force) {
    if (!queued_transition || !queued_transition->force) {
        queued_transition = transition{
            [](engine& eng, scene* prev){ return std::make_shared<T>(eng, prev); },
            force
        };
    }
}

} // namespace ember
