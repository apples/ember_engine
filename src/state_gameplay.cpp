#include "state_gameplay.hpp"

#include <glm/gtx/intersect.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/noise.hpp>
#include <glm/gtx/quaternion.hpp>

#include <tuple>

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

    auto bottomleft = mb.vertex()
        .position({left, bottom, 0})
        .texcoord({0, 0})
        .normal({0, 0, 1})
        .get();

    auto topleft = mb.vertex()
        .position({left, top, 0})
        .texcoord({0, 1})
        .normal({0, 0, 1})
        .get();

    auto bottomright = mb.vertex()
        .position({right, bottom, 0})
        .texcoord({1, 0})
        .normal({0, 0, 1})
        .get();

    auto topright = mb.vertex()
        .position({right, top, 0})
        .texcoord({1, 1})
        .normal({0, 0, 1})
        .get();
    
    mb.tri(bottomleft, topleft, topright);
    mb.tri(topright, bottomright, bottomleft);

    return mb.get();
}

} // namespace

state_gameplay::state_gameplay(
    ember_client_engine& engine,
    resource_cache<sushi::texture_2d, std::string>& texture_cache,
    resource_cache<sushi::mesh_group, std::string>& mesh_cache,
    resource_cache<sushi::skeleton, std::string>& skeleton_cache,
    emberjs::websocket& ws,
    sol::state& lua,
    display_info& display)
    : engine{&engine},
      ws{&ws},
      lua{&lua},
      texture_cache{&texture_cache},
      mesh_cache{&mesh_cache},
      skeleton_cache{&skeleton_cache},
      display{&display},
      floor_mesh{get_floor_mesh()},
      terrain_tex{texture_cache.get("terrain")},
      gui_state{lua.create_table()},
      cam{camera(display.aspect_ratio, 15.f, 1.f, 30.f)},
      field{board()},
      my_player_id{0},
      selected_item_index{0},
      erasing{false},
      placement_dir{direction::RP},
      my_currency{0},
      game_time{0},
      ticks_per_day{1},
      ticks_per_night{1},
      enable_lighting{true} {

    gui_state["buy_item"] = [this](const std::string& item_id) { buy_item(item_id); };
}

void state_gameplay::tick(std::chrono::nanoseconds delta) {
    gui_state["health"] = 3;
    gui_state["max_health"] = 3;
    gui_state["score"] = 0;

    // Scripting system
    ember::run_lua_system("scripting", entities);

    // Render model system

    entities.visit([&](component::render_model& model) {
        if (model.anim_index) {
            model.anim_time += std::chrono::duration_cast<component::render_model::anim_duration_t>(delta * 2);
        }
    });
}

void state_gameplay::render(basic_shader_program& program_basic) {
    auto proj = glm::ortho(-400.f, 400.f, -300.f, 300.f, -1.f, 1.f);
    auto view = glm::mat4(1.f);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    //glEnable(GL_SAMPLE_COVERAGE);
    //glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);

    program_basic.bind();
    program_basic.set_cam_forward({0.0, 0.0, -1.0});
    program_basic.set_tint({1, 1, 1, 1});
    program_basic.set_hue(0);
    program_basic.set_saturation(1);

    renderq.prepare(program_basic, proj, view);

    // Render stacks

    entity.foreach([&](int r, int c, auto& stack) {
        // Sort current stack

        std::sort(begin(stack.entities), end(stack.entities), [](auto& a, auto& b) {
            auto ai = a.eid.get_index();
            auto bi = b.eid.get_index();
            return std::tie(a.pos->stacking_order, ai) < std::tie(b.pos->stacking_order, bi);
        });

        float height = 0.f; /** Height of the current item in the stack */

        // Render sorted stack

        for (auto& e : stack.entities) {
            auto eid = e.eid;
            const auto& pos = *e.pos;

            // Optional components

            ginseng::optional<component::render_model> model;
            ginseng::optional<component::carry> carry;
            ginseng::optional<component::hat> hat;

            if (entities.has_component<component::render_model>(eid)) {
                model = ginseng::optional<component::render_model>(entities.get_component<component::render_model>(eid));
            }

            if (entities.has_component<component::carry>(eid)) {
                carry = ginseng::optional<component::carry>(entities.get_component<component::carry>(eid));
            }

            if (entities.has_component<component::hat>(eid)) {
                hat = ginseng::optional<component::hat>(entities.get_component<component::hat>(eid));
            }

            // Position

            float x = pos.c;
            float y = height;
            float z = pos.r;
            auto rot = glm::angleAxis(glm::radians(get_rotation(pos.dir)), glm::vec3{0.f, 1.f, 0.f});

            // Apply position lerp

            if (entities.has_component<component::position_lerp>(eid)) {
                auto& lerp = entities.get_component<component::position_lerp>(eid);

                if (lerp.y_pos.get_target() != height) {
                    lerp.y_pos.set_target(height, 1.0);
                }

                auto current = lerp.y_pos.get_current();

                y = lerp.y_pos.get_current();
            }

            // Pose

            std::optional<sushi::pose> pose;

            if (model && model->skeleton) {
                auto anim_time_float =
                    std::chrono::duration_cast<std::chrono::duration<float>>(model->anim_time).count();

                pose = sushi::get_pose(*model->skeleton, model->anim_index, anim_time_float, true);
            }

            // Render entity

            if (model && model->mesh && model->texture) {
                auto scale = glm::vec3{1, 1, 1};

                auto tint = glm::vec4{1, 1, 1, 1};

                // Squash resource deposits
                if (entities.has_component<component::item_giver>(eid) && entities.has_component<component::resource_deposit>(eid)) {
                    auto& item_giver = entities.get_component<component::item_giver>(eid);
                    auto& resource_deposit = entities.get_component<component::resource_deposit>(eid);

                    scale.x = 0.5f + 0.5 * float(item_giver.count) / float(resource_deposit.max_count);
                    scale.y = float(item_giver.count) / float(resource_deposit.max_count) + 0.0025f;
                    scale.z = scale.x;
                }

                // Highlight building when erasing
                if (erasing && entities.has_component<component::building>(eid)) {
                    auto root = get_building_root_id(entities, eid);
                    if (root == erasing_id) {
                        tint = glm::vec4{1, 0.2, 0.2, 0.5};
                    }
                }

                for (auto& r_offset : {-field.rows(), 0, field.rows()}) {
                    for (auto& c_offset : {-field.cols(), 0, field.cols()}) {
                        auto item = render_queue::render_item{};
                        item.transform.pos = glm::vec3{x + c_offset, y, z + r_offset};
                        item.transform.rot = rot;
                        item.transform.scl = scale;
                        item.tint = tint;
                        item.mesh = model->mesh;
                        item.texture = model->texture;
                        item.pose = pose;

                        renderq.add_render_item(item);
                    }
                }
            }

            // Render hat

            if (hat) {
                auto mesh_ptr = mesh_cache->get(hat->model);
                auto tex_ptr = texture_cache->get(hat->texture);

                auto attached_to = std::optional<render_queue::render_item::attachment>{};

                // Attach to parent's Hat bone, if available.
                if (model && model->skeleton && pose) {
                    auto bone_idx = sushi::get_bone_index(*model->skeleton, "Hat");

                    if (bone_idx) {
                        attached_to = render_queue::render_item::attachment{
                            *pose,
                            *bone_idx,
                        };
                    }
                }

                for (auto& r_offset : {-field.rows(), 0, field.rows()}) {
                    for (auto& c_offset : {-field.cols(), 0, field.cols()}) {
                        auto item = render_queue::render_item{};
                        item.transform.pos = glm::vec3{x + c_offset, y, z + r_offset};
                        item.transform.rot = rot;
                        item.mesh = mesh_ptr;
                        item.texture = tex_ptr;
                        item.attached_to = attached_to;

                        renderq.add_render_item(item);
                    }
                }
            }

            // Render carried items

            if (carry) {
                for (auto i = 0; i < carry->goods.size(); ++i) {
                    const auto& good = carry->goods[i];

                    if (good.material.empty()) {
                        continue;
                    }

                    auto mesh_ptr = mesh_cache->get("goods/" + good.material);
                    auto tex_ptr = texture_cache->get("goods/" + good.material);

                    auto attached_to = std::optional<render_queue::render_item::attachment>{};

                    // Attach to parent's Carry bone, if available.
                    if (model && model->skeleton && pose) {
                        auto bone_idx = sushi::get_bone_index(*model->skeleton, "Carry." + std::to_string(i+1));

                        if (bone_idx) {
                            attached_to = render_queue::render_item::attachment{
                                *pose,
                                *bone_idx,
                            };
                        }
                    }

                    for (auto& r_offset : {-field.rows(), 0, field.rows()}) {
                        for (auto& c_offset : {-field.cols(), 0, field.cols()}) {
                            auto item = render_queue::render_item{};
                            item.transform.pos = glm::vec3{x + c_offset, y, z + r_offset};
                            item.transform.rot = rot;
                            item.mesh = mesh_ptr;
                            item.texture = tex_ptr;
                            item.attached_to = attached_to;

                            renderq.add_render_item(item);
                        }
                    }
                }
            }

            // Add height

            if (carry) {
                height += pos.height + 0.75; // Entity height + hat height + item height
            } else if (hat) {
                height += pos.height + 0.25; // Entity height + hat height
            } else {
                height += pos.height; // Entity height
            }
        }
    });

    // Draw building preview
    if (show_building_preview) {
        auto item_id = std::get<0>(inventory[selected_item_index]);

        auto iter = std::find_if(begin(items), end(items), [&](auto& item) { return item.id == item_id; });

        if (iter != end(items)) {
            auto& item = *iter;

            auto mesh_ptr = mesh_cache->get(item.mesh);
            auto tex_ptr = texture_cache->get(item.texture);

            auto rot = glm::quat{1, 0, 0, 0};

            if (item.rotates) {
                rot = glm::angleAxis(glm::radians(get_rotation(placement_dir)), glm::vec3(0.f, 1.f, 0.f));
            }

            for (auto& r_offset : {-field.rows(), 0, field.rows()}) {
                for (auto& c_offset : {-field.cols(), 0, field.cols()}) {
                    auto item = render_queue::render_item{};
                    item.transform.pos = glm::vec3{pickx + c_offset, 0.f, pickz + r_offset};
                    item.transform.rot = rot;
                    item.tint = {1, 1, 1, 0.5};
                    item.mesh = mesh_ptr;
                    item.texture = tex_ptr;

                    renderq.add_render_item(item);
                }
            }
        }
    }

    renderq.render_items();
}

void state_gameplay::on_ws_message(const message::message_to_client& msg) {
    auto visitor = utility::overload{
        [&](const message::sync_board& sb) {
            field = sb.board;
            entity_stacks = array2d<entity_stack>(field.rows(), field.cols());
        },
        [&](const message::update_board& ub) {
            field.at(ub.r, ub.c) = ub.tile;
        },
        [&](const message::sync_items& si) {
            items = si.items;
            std::cout << nlohmann::json(si) << std::endl;
            for (auto&& item : items) {
                std::cout << "item " << item.id << ": " << item.name << std::endl;
            }
        },
        [&](const message::sync_shop& ss) {
            shop = ss.items;
            std::cout << nlohmann::json(ss) << std::endl;
            gui_state["shop"] = lua->create_table();

            for (auto i = 0; i < shop.size(); ++i) {
                auto& item = shop[i];
                std::cout << "shop item " << item.item_id << ": " << item.cost << std::endl;
                gui_state["shop"][i + 1] = lua->create_table();
                gui_state["shop"][i + 1]["item_id"] = item.item_id;
                gui_state["shop"][i + 1]["cost"] = item.cost;
            }
        },
        [&](const message::sync_inventory& si) {
            inventory = si.amounts;
            if (selected_item_index >= inventory.size()) {
                selected_item_index = 0;
            }
        },
        [&](const message::update_inventory& ui) {
            auto inv_iter = std::find_if(begin(inventory), end(inventory), [&](const auto& inv) {
                return std::get<0>(inv) == ui.item_id;
            });

            if (inv_iter == end(inventory)) {
                std::cerr << "Tried to update inventory with invalid item: " << nlohmann::json(ui) << std::endl;
                return;
            }

            std::get<1>(*inv_iter) = ui.new_amount;
        },
        [&](const message::create_entity& ce) {
            auto ent = entities.create_entity(ce.id);
            deserialize_component(
                entities, ent, ce.coms, field.rows(), field.cols(), *mesh_cache, *texture_cache, *skeleton_cache);
        },
        [&](const message::update_entity& ce) {
            auto ent = entities.get_or_create_entity(ce.id);
            deserialize_component(
                entities, ent, ce.coms, field.rows(), field.cols(), *mesh_cache, *texture_cache, *skeleton_cache);
        },
        [&](const message::destroy_entity& ce) {
            entities.destroy_entity(ce.id);
        },
        [&](const message::set_position& sp) {
            auto ent = entities.get_entity(sp.id).value();
            auto& pos = entities.get_component<component::position>(ent);
            pos.r = sp.r;
            pos.c = sp.c;
        },
        [&](const message::look_at& look_at) {
            cam.set_position(look_at.r, look_at.c);
        },
        [&](const message::set_player_id& set_player_id) {
            my_player_id = set_player_id.id;
        },
        [&](const message::update_currency& update_currency) {
            my_currency = update_currency.amount;
        },
        [&](const message::update_time& update_time) {
            game_time = update_time.cur_time;
            ticks_per_day = update_time.ticks_per_day;
            ticks_per_night = update_time.ticks_per_night;
        }
    };

    std::visit(utility::overload{
        [&](const message::message_to_client_vec& vec) {
            for (const auto& m : vec) {
                std::visit(visitor, m);
            }
        },
        visitor,
    }, msg);
}

void state_gameplay::place_item(component::position pos) {
    if (selected_item_index >= inventory.size()) {
        return;
    }

    auto msg = message::place_item{};
    msg.pos = pos;
    msg.item_id = std::get<0>(inventory[selected_item_index]);
    engine->send(msg);
}

void state_gameplay::erase_building(ember_database::net_id id) {
    auto msg = message::erase_building{};
    msg.building_id = id;
    engine->send(msg);
}

void state_gameplay::buy_item(const std::string& item_id) {
    auto msg = message::buy_item{};
    msg.item_id = item_id;
    engine->send(msg);
}

auto state_gameplay::pick_tile_at(float screen_x, float screen_y) const -> location {
    auto proj = cam.get_proj_mat();
    auto view = cam.get_view_mat();

    auto mpos = glm::vec3(glm::inverse(proj*view) * glm::vec4(screen_x, screen_y, 1.0, 1.0));
    auto forward = -glm::vec3(glm::row(view, 2));

    float distance;
    glm::intersectRayPlane(mpos, forward, glm::vec3(0,0,0), glm::vec3(0,1,0), distance);

    auto intersection = mpos + forward*distance;

    auto ret = location{};

    ret.col = std::floor(intersection.x + 0.5);
    ret.row = std::floor(intersection.z + 0.5);

    return ret;
}

auto state_gameplay::mouse_pick_tile() const -> location {
    if (field.cols() == 0 || field.rows() == 0) {
        return location{};
    }

    int mx, my;
    SDL_GetMouseState(&mx, &my);

    auto pick = pick_tile_at(mx / float(display->width / 2) - 1.0, 1.0 - my / float(display->height / 2));

    pick.col = imod(pick.col, field.cols());
    pick.row = imod(pick.row, field.rows());

    return pick;
}

void state_gameplay::event_click(const SDL_MouseButtonEvent& e) {
    switch (e.button) {
        case SDL_BUTTON_LEFT: {
            auto [pickx, pickz] = mouse_pick_tile();

            if (pickx >= 0 && pickx < field.cols() && pickz >= 0 && pickz < field.rows()) {
                if (erasing) {
                    auto building_id = field.at(pickz, pickx).building_id;
                    if (building_id != 0) {
                        auto eid = entities.get_entity(building_id);
                        if (eid) {
                            auto erasing_id = get_building_root_id(entities, *eid);
                            auto erasing_eid = entities.get_entity(erasing_id);
                            if (erasing_eid && entities.has_component<component::building>(*erasing_eid)) {
                                auto& building = entities.get_component<component::building>(*erasing_eid);
                                if (building.owner_id == my_player_id && building.refundable) {
                                    erase_building(erasing_id);
                                }
                            }
                        }
                    }
                } else {
                    place_item({pickz, pickx, placement_dir});
                }
            }
            break;
        }
    }
}

auto state_gameplay::handle_game_input(const SDL_Event& event) -> bool {
    switch (event.type) {
        case SDL_KEYDOWN:
            switch (event.key.keysym.scancode) {
                case SDL_SCANCODE_1: select_item(0); break;
                case SDL_SCANCODE_2: select_item(1); break;
                case SDL_SCANCODE_3: select_item(2); break;
                case SDL_SCANCODE_4: select_item(3); break;
                case SDL_SCANCODE_5: select_item(4); break;
                case SDL_SCANCODE_6: select_item(5); break;
                case SDL_SCANCODE_7: select_item(6); break;
                case SDL_SCANCODE_8: select_item(7); break;
                case SDL_SCANCODE_9: select_item(8); break;
                case SDL_SCANCODE_Q: placement_dir = rotate_left(placement_dir); break;
                case SDL_SCANCODE_E: placement_dir = rotate_right(placement_dir); break;
                case SDL_SCANCODE_X: select_eraser(); break;
                case SDL_SCANCODE_L: enable_lighting = !enable_lighting; break;
                default: return false;
            }
            return true;
        case SDL_MOUSEWHEEL:
            cam.relative_zoom(std::pow(1.1, event.wheel.y == 0 ? 0 : event.wheel.y > 0 ? -1 : 1));
            return true;
        case SDL_MOUSEBUTTONDOWN:
            event_click(event.button);
            return true;
    }

    return false;
}

void state_gameplay::select_item(int i) {
    if (i < inventory.size()) {
        selected_item_index = i;
        erasing = false;
    }
}

void state_gameplay::select_eraser() {
    erasing = true;
}

auto state_gameplay::render_gui() -> sol::table {
    auto module_name = "gui.state_gameplay";
    auto file_name = "data/scripts/gui/state_gameplay/init.lua";
    auto component = lua->require_file(module_name, file_name);
    auto element = lua->create_table();
    element["type"] = component;
    element["props"] = gui_state;
    element["children"] = lua->create_table();
    return element;
}