#ifndef CLIENT_STATE_MAINMENU_HPP
#define CLIENT_STATE_MAINMENU_HPP

#include "state.hpp"

#include "emberjs/hubservice.hpp"
#include "emberjs/websocket.hpp"

#include "components.hpp"
#include "camera.hpp"
#include "config.hpp"
#include "display.hpp"
#include "engine.hpp"
#include "shaders.hpp"

#include <common/array2d.hpp>
#include <common/board.hpp>
#include <common/entities.hpp>
#include <common/messages.hpp>
#include <common/utility.hpp>
#include <common/building.hpp>
#include <common/resource_cache.hpp>

#include <sushi/sushi.hpp>

#include <sol.hpp>

#include <cmath>
#include <memory>
#include <tuple>
#include <vector>

class state_mainmenu final : public ember::state {
public:
    state_mainmenu(
        ember_client_engine& engine,
        emberjs::websocket& ws,
        sol::state& lua,
        emberjs::hubservice& hubservice);

    state_mainmenu(const state_mainmenu&) = delete;
    state_mainmenu(state_mainmenu&&) = delete;
    state_mainmenu& operator=(const state_mainmenu&) = delete;
    state_mainmenu& operator=(state_mainmenu&&) = delete;

    virtual void on_event(const events::event& e) override;
    virtual void tick(std::chrono::nanoseconds delta_time, double delta, const Uint8* keys) override;
    virtual void render(basic_shader_program& program_basic) override;
    virtual void on_ws_message(const message::message_to_client& msg) override;
    virtual auto handle_game_input(const SDL_Event& event) -> bool override;
    virtual auto render_gui() -> sol::table override;

private:
    void create_new_game(const std::string& name);
    void join_game(const std::string& url);
    void refresh();

    ember_client_engine* engine;
    sol::state* lua;
    emberjs::websocket* ws;
    emberjs::hubservice* hubservice;
    sol::table gui_state;
    emberjs::hubservice_cancels cancels;
};

#endif // CLIENT_STATE_MAINMENU_HPP