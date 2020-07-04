#include "scene_gameplay.hpp"

#include "components.hpp"

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
    auto proj = glm::ortho(-400.f, 400.f, -300.f, 300.f, -1.f, 1.f);
    auto view = glm::mat4(1.f);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    //glEnable(GL_SAMPLE_COVERAGE);
    //glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);

    engine->basic_shader.bind();
    engine->basic_shader.set_cam_forward({0.0, 0.0, -1.0});
    engine->basic_shader.set_tint({1, 1, 1, 1});
    engine->basic_shader.set_hue(0);
    engine->basic_shader.set_saturation(1);

    renderq.prepare(engine->basic_shader, proj, view);

    // Render entities

    entities.visit([&](const component::render_model& model, const component::transform& transform) {
        if (model.mesh && model.texture) {
            // Pose

            std::optional<sushi::pose> pose;

            if (model.skeleton) {
                pose = sushi::get_pose(*model.skeleton, model.anim_index, model.anim_time, true);
            }

            // Render

            auto item = ember::render_queue::item{};
            item.transform = transform;
            item.mesh = model.mesh;
            item.texture = model.texture;
            item.pose = pose;

            renderq.add(item);
        }
    });

    renderq.render_items();
}

auto scene_gameplay::handle_game_input(const SDL_Event& event) -> bool {
    return false;
}

auto scene_gameplay::render_gui() -> sol::table {
    return ember::vdom::create_element(engine->lua, "gui.scene_gameplay.root", gui_state, engine->lua.create_table());
}
