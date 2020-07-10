#include "scene_gameplay.hpp"

#include "scene_mainmenu.hpp"
#include "components.hpp"
#include "meshes.hpp"

#include "ember/camera.hpp"
#include "ember/engine.hpp"
#include "ember/vdom.hpp"

#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>

#include <sushi/sushi.hpp>

// Scene constructor
// Initializes private members and loads any prerequisite assets (usually small ones!).
// Scene is not actually the current scene yet, so interactions with the engine state and rendering are forbidden.
scene_gameplay::scene_gameplay(ember::engine& engine, ember::scene* prev)
    : scene(engine),
      camera(), // Camera has a sane default constructor, it is tweaked below
      entities(), // Entity database has no constructor parameters
      physics(), // Physics system
      gui_state{engine.lua.create_table()}, // Gui state is initialized to an empty Lua table
      sprite_mesh{get_sprite_mesh()}, // Sprite and tilemap meshes is created statically
      tilemap_mesh{get_tilemap_mesh()},
      lives(3) {
    camera.height = 32; // Height of the camera viewport in world units, in this case 32 tiles
    camera.near = -1; // Near plane of an orthographic view is away from camera, so this is actually +1 view range on Z
}

// Scene initialization
// Distinct from construction, all private members should have valid values now.
// Used to initialize Lua state, initialize world, populate entities, etc.
// Basically "load the stage".
void scene_gameplay::init() {
    // We want scripts to have access to the entities as a global variable, so it is set here.
    engine->lua["entities"] = std::ref(entities);
    engine->lua["lose_life"] = [this]{
        if (lives == 0) {
            engine->queue_transition<scene_mainmenu>();
        } else {
            lives -= 1;
        }
    };
    // Call the "init" function in the "data/scripts/scenes/gameplay.lua" script, with no params.
    engine->call_script("scenes.gameplay", "init");
}

// Tick/update function
// Performs all necessary game processing based on the delta time.
// Updates gui_state as necessary.
// Basically does everything except rendering.
void scene_gameplay::tick(float delta) {
    gui_state["health"] = lives;
    gui_state["max_health"] = 3;
    gui_state["score"] = 0;

    // Scripting system
    engine->call_script("systems.scripting", "visit", delta);

    // Sprite system
    entities.visit([&](component::sprite& sprite) {
        sprite.time += delta;
    });

    // Physics system
    physics.step(*engine, entities, delta);
}

// Render function
// Notice the lack of delta time.
// Performs all world rendering, but not GUI.
// Responsible for setting up the OpenGL state, binding shaders, etc.
// Must be *fast*!
// The EZ3D helper can be used for efficient 3D rendering, currently there is no equivalent for 2D.
void scene_gameplay::render() {
    // Set up some OpenGL state. This is mostly copy-pasted and should be self-explanatory.
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glEnable(GL_SAMPLE_COVERAGE);
    //glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);

    // Bind the basic shader and set some defaults.
    engine->basic_shader.bind();
    engine->basic_shader.set_cam_forward(get_forward(camera));
    engine->basic_shader.set_tint({1, 1, 1, 1});
    engine->basic_shader.set_hue(0);
    engine->basic_shader.set_saturation(1);

    // Get projection and view matrices from camera.
    auto proj = get_proj(camera);
    auto view = get_view(camera);

    // Render tilemap
    {
        auto modelmat = glm::mat4(1.f);

        engine->basic_shader.set_uvmat(glm::mat3{1.f});
        engine->basic_shader.set_normal_mat(glm::inverseTranspose(view * modelmat));
        engine->basic_shader.set_MVP(proj * view * modelmat);

        sushi::set_texture(0, *engine->texture_cache.get("sprites"));
        sushi::draw_mesh(tilemap_mesh);
    }

    // Render entities
    entities.visit([&](const component::sprite& sprite, const component::transform& transform) {
        auto modelmat = to_mat4(transform);
        auto tex = engine->texture_cache.get(sprite.texture);

        // Calculate UV matrix for rendering the correct sprite.
        auto cols = int(1 / sprite.size.x);
        auto frame = sprite.frames[int(sprite.time * 12) % sprite.frames.size()];
        auto uvoffset = glm::vec2{frame % cols, frame / cols} * sprite.size;
        auto uvmat = glm::mat3{1.f};
        uvmat = glm::translate(uvmat, uvoffset);
        uvmat = glm::scale(uvmat, sprite.size);

        // Set matrix uniforms.
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

// Handle input events, called asynchronously
auto scene_gameplay::handle_game_input(const SDL_Event& event) -> bool {
    using component::controller;

    // Updates a single key for all controllers
    auto update = [&](bool press, bool controller::*key, bool controller::*pressed) {
        entities.visit([&](controller& con) {
            if (key) con.*key = press;
            if (pressed) con.*pressed = press;
        });
    };

    // Processes a key event
    auto process_key = [&](const SDL_KeyboardEvent& key){
        auto pressed = key.state == SDL_PRESSED;
        switch (key.keysym.sym) {
            case SDLK_LEFT:
                update(pressed, &controller::left, nullptr);
                return true;
            case SDLK_RIGHT:
                update(pressed, &controller::right, nullptr);
                return true;
            case SDLK_SPACE:
                update(pressed, nullptr, &controller::action_pressed);
                return true;
            default:
                return false;
        }
    };

    switch (event.type) {
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            return process_key(event.key);
    }
    return false;
}

// Render GUI, which means returning the result of `create_element`.
auto scene_gameplay::render_gui() -> sol::table {
    return ember::vdom::create_element(engine->lua, "gui.scene_gameplay.root", gui_state, engine->lua.create_table());
}
