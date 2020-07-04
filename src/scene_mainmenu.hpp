#pragma once

#include "ember/scene.hpp"

#include <sol.hpp>

class scene_mainmenu final : public ember::scene {
public:
    scene_mainmenu(ember::engine& eng, ember::scene* prev);

    virtual void init() override;
    virtual void tick(float delta) override;
    virtual void render() override;
    virtual auto handle_game_input(const SDL_Event& event) -> bool override;
    virtual auto render_gui() -> sol::table override;

private:
    void start_game();

    sol::table gui_state;
};
