#include "state_mainmenu.hpp"

#include <iostream>

state_mainmenu::state_mainmenu(
    ember_client_engine& engine,
    emberjs::websocket& ws,
    sol::state& lua,
    emberjs::hubservice& hubservice)
    : engine(&engine),
      ws{&ws},
      lua{&lua},
      hubservice{&hubservice},
      gui_state{lua.create_table()} {
    gui_state["gamelist"] = nullptr;
    gui_state["new_game"] = [this](const std::string& name){ create_new_game(name); };
    gui_state["join_game"] = [this](const std::string& url){ join_game(url); };
    gui_state["refresh"] = [this](){ refresh(); };
    gui_state["clear_error"] = [this](){ gui_state["error_message"] = nullptr; };

    std::cout << (void*)this << "," << gui_state.registry_index() << std::endl;

    refresh();
}

void state_mainmenu::on_event(const events::event& e) {
    std::visit(
        utility::overload(
            [this](const events::ws_connected&) {
                if (auto token = hubservice->get_token()) {
                    std::clog << "Logging in." << std::endl;
                    engine->send(message::login{*token});
                    engine->to_gameplay();
                }
            },
            [this](const events::ws_disconnected& dc) {
                std::clog << "Disconnected: \"" << dc.reason << "\"." << std::endl;
                gui_state["error_message"] = dc.reason;
            },
            [](auto&&) {}),
        e);
}

void state_mainmenu::tick(std::chrono::nanoseconds delta_time, double delta, const Uint8* keys) {}

void state_mainmenu::render(basic_shader_program& program_basic) {}

void state_mainmenu::on_ws_message(const message::message_to_client& msg) {}

auto state_mainmenu::handle_game_input(const SDL_Event& event) -> bool {
    return false;
}

auto state_mainmenu::render_gui() -> sol::table {
    auto module_name = "gui.state_mainmenu";
    auto file_name = "data/scripts/gui/state_mainmenu/init.lua";
    auto component = lua->require_file(module_name, file_name);
    auto element = lua->create_table();
    element["type"] = component;
    element["props"] = gui_state;
    element["children"] = lua->create_table();
    return element;
}

void state_mainmenu::create_new_game(const std::string& name) {
    gui_state["creating"] = true;
    hubservice->gameserver_new_game(
        cancels.make(),
        name,
        [this](const std::string& url) {
            gui_state["creating"] = false;
            ws->connect(url);
        },
        [this](const std::string& error) {
            gui_state["creating"] = false;
            gui_state["error_message"] = error;
        });
}

void state_mainmenu::join_game(const std::string& url) {
    ws->connect(url);
}

void state_mainmenu::refresh() {
    gui_state["refreshing"] = true;
    hubservice->gameserver_list(
        cancels.make(),
        [this](const auto& servers) {
            auto luaservers = std::vector<sol::table>{};
            luaservers.reserve(servers.size());

            for (const auto& s : servers) {
                auto ls = this->lua->create_table();
                ls["url"] = s.url;
                ls["title"] = s.title;
                luaservers.push_back(ls);
            }

            gui_state["gamelist"] = luaservers;
            gui_state["refreshing"] = false;
        },
        [this](const std::string& error) {
            std::cout << "Error listing: " << error << std::endl;
            std::cout << (void*)this << "," << gui_state.registry_index() << std::endl;
            gui_state["error_message"] = error;
            gui_state["refreshing"] = false;
        });
}