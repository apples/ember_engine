#include "scene_gameplay.hpp"

#include "components.hpp"

#include "ember/camera.hpp"
#include "ember/engine.hpp"
#include "ember/vdom.hpp"

#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>

#include <sushi/sushi.hpp>

namespace {

auto get_sprite_mesh() -> sushi::mesh_group {
    sushi::mesh_group_builder mb;
    mb.enable(sushi::attrib_location::POSITION);
    mb.enable(sushi::attrib_location::TEXCOORD);
    mb.enable(sushi::attrib_location::NORMAL);

    mb.mesh("sprite");

    auto left = -0.5f;
    auto right = 0.5f;
    auto bottom = -0.5f;
    auto top = 0.5f;

    auto bottomleft = mb.vertex().position({left, bottom, 0}).texcoord({0, 1}).normal({0, 1, 0}).get();
    auto topleft = mb.vertex().position({left, top, 0}).texcoord({0, 0}).normal({0, 1, 0}).get();
    auto bottomright = mb.vertex().position({right, bottom, 0}).texcoord({1, 1}).normal({0, 1, 0}).get();
    auto topright = mb.vertex().position({right, top, 0}).texcoord({1, 0}).normal({0, 1, 0}).get();

    mb.tri(bottomleft, topleft, topright);
    mb.tri(topright, bottomright, bottomleft);

    return mb.get();
}

} // namespace

scene_gameplay::scene_gameplay(ember::engine& engine, ember::scene* prev)
    : scene(engine), camera(), entities(), gui_state{engine.lua.create_table()}, sprite_mesh{get_sprite_mesh()} {
    camera.height = 32;
    camera.near = -1;
}

void scene_gameplay::init() {
    engine->lua["entities"] = std::ref(entities);
    engine->call_script("scenes.gameplay", "init");
}

void scene_gameplay::tick(float delta) {
    gui_state["health"] = 3;
    gui_state["max_health"] = 3;
    gui_state["score"] = 0;

    // Scripting system
    engine->call_script("systems.scripting", "visit", delta);

    // Sprite system
    entities.visit([&](component::sprite& sprite) {
        sprite.time += delta;
    });
}

void scene_gameplay::render() {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    //glEnable(GL_SAMPLE_COVERAGE);
    //glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);

    engine->basic_shader.bind();
    engine->basic_shader.set_cam_forward(get_forward(camera));
    engine->basic_shader.set_tint({1, 1, 1, 1});
    engine->basic_shader.set_hue(0);
    engine->basic_shader.set_saturation(1);

    auto proj = get_proj(camera);
    auto view = get_view(camera);

    entities.visit([&](const component::sprite& sprite, const component::transform& transform) {
        auto modelmat = to_mat4(transform);
        auto tex = engine->texture_cache.get(sprite.texture);
        auto cols = int(1 / sprite.size.x);
        auto frame = sprite.frames[int(sprite.time * 12) % sprite.frames.size()];
        auto uvoffset = glm::vec2{frame % cols, frame / cols} * sprite.size;
        auto uvmat = glm::mat3{1.f};
        uvmat = glm::translate(uvmat, uvoffset);
        uvmat = glm::scale(uvmat, sprite.size);

        engine->basic_shader.set_uvmat(uvmat);
        engine->basic_shader.set_normal_mat(glm::inverseTranspose(view * modelmat));
        engine->basic_shader.set_MVP(proj * view * modelmat);

        if (tex) {
            sushi::set_texture(0, *tex);
        } else {
            std::cout << "Warning: Texture not found: " << sprite.texture << std::endl;
        }
        
        sushi::draw_mesh(sprite_mesh);
    });
}

auto scene_gameplay::handle_game_input(const SDL_Event& event) -> bool {
    return false;
}

auto scene_gameplay::render_gui() -> sol::table {
    return ember::vdom::create_element(engine->lua, "gui.scene_gameplay.root", gui_state, engine->lua.create_table());
}
