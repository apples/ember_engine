#ifndef CLIENT_STATE_GAMEPLAY_HPP
#define CLIENT_STATE_GAMEPLAY_HPP

#include "components.hpp"
#include "display.hpp"
#include "shaders.hpp"
#include "render_queue.hpp"

#include "ember/entities.hpp"
#include "ember/engine.hpp"
#include "ember/utility.hpp"
#include "ember/resource_cache.hpp"
#include "ember/state.hpp"

#include <sushi/sushi.hpp>
#include <sol.hpp>

#include <cmath>
#include <memory>
#include <tuple>
#include <vector>

class state_gameplay final : public state {
public:
    state_gameplay(
        ember_client_engine& engine,
        resource_cache<sushi::texture_2d, std::string>& texture_cache,
        resource_cache<sushi::mesh_group, std::string>& mesh_cache,
        resource_cache<sushi::skeleton, std::string>& skeleton_cache,
        emberjs::websocket& ws,
        sol::state& lua,
        display_info& display);

    virtual void on_event(const events::event& e) override;
    virtual void tick(std::chrono::nanoseconds delta_time, double delta, const Uint8* keys) override;
    virtual void render(basic_shader_program& program_basic) override;
    virtual void on_ws_message(const message::message_to_client& msg) override;
    virtual auto handle_game_input(const SDL_Event& event) -> bool override;
    virtual auto render_gui() -> sol::table override;

private:
    void place_item(component::position pos);
    void erase_building(ember_database::net_id id);
    void buy_item(const std::string& item_id);

    auto pick_tile_at(float screen_x, float screen_y) const -> location;
    auto mouse_pick_tile() const -> location;
    void select_item(int i);
    void select_eraser();
    void event_click(const SDL_MouseButtonEvent& e);

    ember::engine* engine;
    sol::state* lua;
    ember::resource_cache<sushi::texture_2d, std::string>* texture_cache;
    ember::resource_cache<sushi::mesh_group, std::string>* mesh_cache;
    ember::resource_cache<sushi::skeleton, std::string>* skeleton_cache;
    display_info* display;

    render_queue renderq;
    sol::table gui_state;
    ember_database entities;
    sushi::mesh_group sprite_mesh;
};

#endif // CLIENT_STATE_GAMEPLAY_HPP