#pragma once

#include "physics.hpp"

#include "ember/camera.hpp"
#include "ember/entities.hpp"
#include "ember/scene.hpp"

#include <sushi/sushi.hpp>
#include <sol.hpp>

#include <cmath>
#include <memory>
#include <tuple>
#include <vector>

class scene_gameplay final : public ember::scene {
public:
    scene_gameplay(ember::engine& eng, scene* prev);

    virtual void init() override;
    virtual void tick(float delta) override;
    virtual void render() override;
    virtual auto handle_game_input(const SDL_Event& event) -> bool override;
    virtual auto render_gui() -> sol::table override;

private:
    ember::camera::orthographic camera;
    ember::database entities;
    physics_system physics;
    sol::table gui_state;
    sushi::mesh_group sprite_mesh;
    sushi::mesh_group tilemap_mesh;
    int lives;
};
