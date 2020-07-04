#ifndef CLIENT_STATE_GAMEPLAY_HPP
#define CLIENT_STATE_GAMEPLAY_HPP

#include "ember/render_queue.hpp"
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
    ember::render_queue renderq;
    ember::database entities;
    sol::table gui_state;
};

#endif // CLIENT_STATE_GAMEPLAY_HPP