#include "scene_mainmenu.hpp"

#include "scene_gameplay.hpp"

#include "ember/engine.hpp"
#include "ember/vdom.hpp"

scene_mainmenu::scene_mainmenu(ember::engine& engine, ember::scene* prev)
    : scene(engine), gui_state{engine.lua.create_table()} {
    gui_state["start_game"] = [this]{ start_game(); };
}

void scene_mainmenu::init() {}

void scene_mainmenu::tick(float delta) {}

void scene_mainmenu::render() {}

auto scene_mainmenu::handle_game_input(const SDL_Event& event) -> bool {
    return false;
}

auto scene_mainmenu::render_gui() -> sol::table {
    return ember::vdom::create_element(engine->lua, "gui.scene_mainmenu.root", gui_state, engine->lua.create_table());
}

void scene_mainmenu::start_game() {
    engine->queue_transition<scene_gameplay>();
}
