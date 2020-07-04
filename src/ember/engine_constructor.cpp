#include "engine.hpp"

#include "math.hpp"
#include "lua_gui.hpp"
#include "component_common.hpp"
#include "scripting.hpp"
#include "entities.hpp"

#include <sol.hpp>

namespace ember {

engine::engine(const config::config& config) {
    std::clog << "Constructing engine..." << std::endl;

    // Initialize Lua

    lua.open_libraries(
        sol::lib::base,
        sol::lib::math,
        sol::lib::string,
        sol::lib::table,
        sol::lib::debug,
        sol::lib::package);

    lua["package"]["path"] = "data/scripts/?.lua;data/scripts/?/init.lua";
    lua["package"]["cpath"] = "";

    sol::table globals = lua.globals();
    math::register_types(globals);
    lua_gui::register_types(globals);
    scripting::register_type<database>(globals);
    register_engine_module();

    {
        auto component_table = lua.create_named_table("component");
        component::register_all_components(component_table);
    }

    gui_state = lua.create_named_table("gui_state");
    gui_state["fps"] = 0;
    gui_state["version"] = "ALPHA 0.0.0";

    // Load config

    display.width = config.display.width;
    display.height = config.display.height;
    display.aspect_ratio = float(display.width) / float(display.height);

    // Initialize window

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    window = SDL_CreateWindow(
        "Ember",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        display.width,
        display.height,
        SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);

    glcontext = SDL_GL_CreateContext(window);

    SDL_StartTextInput();

    // Compile Shaders

    basic_shader = shaders::basic_shader_program("data/shaders/basic.vert", "data/shaders/basic.frag");
    msdf_shader = shaders::msdf_shader_program("data/shaders/msdf.vert", "data/shaders/msdf.frag");

    basic_shader.bind();
    basic_shader.set_s_texture(0);

    // Resource caches

    mesh_cache = [](const std::string& name) -> sushi::mesh_group {
        auto load_default = []() -> sushi::mesh_group {
            auto iqm = sushi::iqm::load_iqm("data/models/default.iqm");
            if (!iqm) {
                std::cerr << "ERROR: Failed to load default IQM mesh!\n";
                return {};
            }

            auto mesh = sushi::load_meshes(*iqm);
            if (mesh.meshes.empty()) {
                std::cerr << "ERROR: Default IQM file does not contain any meshes\n";
            }

            return mesh;
        };

        auto iqm = sushi::iqm::load_iqm("data/models/" + name + ".iqm");
        if (!iqm) {
            std::cerr << "ERROR: Failed to load IQM mesh \"" << name << "\", loading default\n";
            return load_default();
        }

        auto mesh = sushi::load_meshes(*iqm);
        if (mesh.meshes.empty()) {
            std::cerr << "ERROR: IQM file \"" << name << "\" does not contain any meshes\n";
        }

        return mesh;
    };

    skeleton_cache = [](const std::string& name) -> std::shared_ptr<sushi::skeleton> {
        auto iqm = sushi::iqm::load_iqm("data/models/" + name + ".iqm");
        if (!iqm) {
            std::cerr << "Warning: Failed to load IQM skeleton \"" << name << "\"\n";
            return nullptr;
        }

        auto skele = sushi::load_skeleton(*iqm);
        if (skele.bones.empty()) {
            std::cerr << "Warning: IQM file \"" << name << "\" does not contain a skeleton\n";
            return nullptr;
        }

        for (const auto& b : skele.bones) {
            std::cout << "Found bone: " << b.name << "\n";
        }

        return std::make_shared<sushi::skeleton>(std::move(skele));
    };

    texture_cache = [](const std::string& name) {
        if (name == ":white") {
            unsigned char white[4] = { 0xff, 0xff, 0xff, 0xff };
            auto tex = sushi::create_uninitialized_texture_2d(1, 1, sushi::TexType::COLORA);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, white);
            return tex;
        } else {
            auto tex = sushi::load_texture_2d("data/textures/" + name + ".png", true, false, true, false);
            if (tex.handle) {
                return tex;
            } else {
                return sushi::load_texture_2d("data/textures/default.png", false, false, true, false);
            }
        }
    };

    font_cache = [](const std::string& fontname) {
        return msdf_font("data/fonts/"+fontname+".ttf");
    };

    // Init GUI

    renderer = sushi_renderer(
        {display.width, display.height},
        basic_shader,
        msdf_shader,
        font_cache,
        mesh_cache,
        texture_cache);

    root_widget = std::make_shared<gui::widget>(renderer);
    root_widget->set_attribute("width", std::to_string(display.width));
    root_widget->set_attribute("height", std::to_string(display.height));

    lua["root_widget"] = root_widget;
    lua["focus_widget"] = [this](gui::widget* widget) {
        focused_widget = widget->weak_from_this();
    };

    auto init_gui_result = lua.do_file("data/scripts/init_gui.lua");

    if (!init_gui_result.valid()) {
        sol::error err = init_gui_result;
        throw std::runtime_error(std::string("init_gui(): ") + err.what());
    }

    update_gui_state = lua["update_gui_state"];

    // Timer setup

    prev_time = clock::now();
    framerate_buffer.reserve(10);

    std::clog << "Engine constructed." << std::endl;
}

engine::~engine() {
    SDL_GL_DeleteContext(glcontext);
    SDL_DestroyWindow(window);
}

} // namespace ember
