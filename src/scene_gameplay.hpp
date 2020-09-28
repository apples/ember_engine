#pragma once

#include "ember/box2d_helpers.hpp"
#include "ember/camera.hpp"
#include "ember/entities.hpp"
#include "ember/scene.hpp"

#include <sushi/sushi.hpp>
#include <sol.hpp>
#include <box2d/box2d.h>

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
    class contact_listener : public b2ContactListener {
    public:
        contact_listener(scene_gameplay& scene);
        virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override;
    private:
        scene_gameplay* scene;
    };

    ember::camera::orthographic camera;
    ember::database entities;
    b2World world;
    contact_listener world_contact_listener;
    ember::box2d_fixed_timestep timestep;
    sol::table gui_state;
    sushi::mesh_group sprite_mesh;
    sushi::mesh_group tilemap_mesh;
    int lives;
    std::vector<ember::database::ent_id> destroy_queue;
};
