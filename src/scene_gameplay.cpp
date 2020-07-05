#include "scene_gameplay.hpp"

#include "components.hpp"

#include "ember/camera.hpp"
#include "ember/engine.hpp"
#include "ember/vdom.hpp"

scene_gameplay::scene_gameplay(ember::engine& engine, ember::scene* prev)
    : scene(engine), gui_state{engine.lua.create_table()} {}

void scene_gameplay::init() {
    engine->lua["entities"] = std::ref(entities);
}

void scene_gameplay::tick(float delta) {
    gui_state["health"] = 3;
    gui_state["max_health"] = 3;
    gui_state["score"] = 0;

    // Scripting system
    engine->call_script("systems.scripting", "visit", delta);

    // Render model system
    entities.visit([&](component::render_model& model) {
        if (model.anim_index) {
            model.anim_time += delta * 2;
        }
    });
}

void scene_gameplay::render() {
    auto cam = ember::camera::perspective{};

    renderer.begin(engine, cam);

    entities.visit([&](const component::render_model& model, const component::transform& transform) {
        renderer.add(model, transform);
    });

    renderer.finish();
}

auto scene_gameplay::handle_game_input(const SDL_Event& event) -> bool {
    return false;
}

auto scene_gameplay::render_gui() -> sol::table {
    return ember::vdom::create_element(engine->lua, "gui.scene_gameplay.root", gui_state, engine->lua.create_table());
}
