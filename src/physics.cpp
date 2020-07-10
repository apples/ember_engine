#include "physics.hpp"

#include "components.hpp"

void physics_system::step(ember::engine& engine, ember::database& entities, float delta) {
    time += delta;

    delta = 1.f/240.f;

    auto n_steps = int(time / delta);

    for (int i = 0; i < n_steps; ++i) {
        using type_t = component::body::type_t;

        struct manifold {
            float left, right, bottom, top;
        };

        struct world_object : manifold {
            ember::database::ent_id eid;
            component::body* body;
            component::transform* transform;
        };

        std::vector<world_object> objects;
        objects.reserve(entities.count_components<component::body>());

        entities.visit([&](ember::database::ent_id eid, component::body& body, component::transform& transform){
            transform.pos += glm::vec3{body.vel * delta, 0};
            objects.push_back({
                {
                    transform.pos.x - body.size.x/2,
                    transform.pos.x + body.size.x/2,
                    transform.pos.y - body.size.y/2,
                    transform.pos.y + body.size.y/2,
                },
                eid,
                &body,
                &transform,
            });
        });

        std::sort(begin(objects), end(objects), [](auto& a, auto& b) {
            return a.left < b.left;
        });

        std::vector<world_object*> current;
        current.reserve(objects.size());

        for (auto& obj : objects) {
            current.erase(std::remove_if(begin(current), end(current), [&](auto* a){
                return a->right < obj.left || !entities.exists(a->eid);
            }), end(current));

            for (auto& obj2 : current) {
                auto overlap = manifold{
                    std::max(obj.left, obj2->left),
                    std::min(obj.right, obj2->right),
                    std::max(obj.bottom, obj2->bottom),
                    std::min(obj.top, obj2->top),
                };

                auto w = overlap.right - overlap.left;
                auto h = overlap.top - overlap.bottom;

                if (w > 0 && h > 0) {
                    auto& a = obj;
                    auto& b = *obj2;
                    if (b.body->type < a.body->type) {
                        std::swap(a, b);
                    }

                    // Pre-collide
                    if (auto b_script = entities.get_component<component::script*>(b.eid)) {
                        engine.call_script("systems.physics", "pre_collide", "actors."+b_script->name, b.eid, a.eid);
                        if (!entities.exists(obj.eid)) {
                            break;
                        }
                        if (!entities.exists(obj2->eid)) {
                            continue;
                        }
                    }
                    if (auto a_script = entities.get_component<component::script*>(a.eid)) {
                        engine.call_script("systems.physics", "pre_collide", "actors."+a_script->name, a.eid, b.eid);
                        if (!entities.exists(obj.eid)) {
                            break;
                        }
                        if (!entities.exists(obj2->eid)) {
                            continue;
                        }
                    }

                    // Collide
                    if (a.body->type == type_t::DYNAMIC && b.body->type == type_t::DYNAMIC) {
                        if (w < h) {
                            auto resolve_x = w/2;
                            if (a.transform->pos.x < b.transform->pos.x) {
                                resolve_x = -resolve_x;
                            }
                            a.transform->pos.x += resolve_x;
                            b.transform->pos.x -= resolve_x;
                            std::swap(a.body->vel.x, b.body->vel.x);
                            a.left += resolve_x;
                            a.right += resolve_x;
                            b.left -= resolve_x;
                            b.right -= resolve_x;
                        } else {
                            auto resolve_y = h/2;
                            if (a.transform->pos.y < b.transform->pos.y) {
                                resolve_y = -resolve_y;
                            }
                            a.transform->pos.y += resolve_y;
                            b.transform->pos.y -= resolve_y;
                            std::swap(a.body->vel.y, b.body->vel.y);
                            a.left += resolve_y;
                            a.right += resolve_y;
                            b.left -= resolve_y;
                            b.right -= resolve_y;
                        }
                    } else if (b.body->type == type_t::DYNAMIC) {
                        if (w < h) {
                            auto resolve_x = w;
                            if (a.transform->pos.x < b.transform->pos.x) {
                                resolve_x = -resolve_x;
                            }
                            b.transform->pos.x -= resolve_x;
                            b.body->vel.x *= -1;
                            b.body->vel.y += a.body->vel.y / 2;
                            b.left -= resolve_x;
                            b.right -= resolve_x;
                        } else {
                            auto resolve_y = h;
                            if (a.transform->pos.y < b.transform->pos.y) {
                                resolve_y = -resolve_y;
                            }
                            b.transform->pos.y -= resolve_y;
                            b.body->vel.x += a.body->vel.x / 2;
                            b.body->vel.y *= -1;
                            b.left -= resolve_y;
                            b.right -= resolve_y;
                        }
                    } else if (a.body->type == type_t::KINEMATIC && b.body->type == type_t::KINEMATIC) {
                        throw std::runtime_error("Not implemented!");
                    } else if (b.body->type == type_t::KINEMATIC) {
                        if (w < h) {
                            auto resolve_x = w;
                            if (a.transform->pos.x < b.transform->pos.x) {
                                resolve_x = -resolve_x;
                            }
                            b.transform->pos.x -= resolve_x;
                            b.left -= resolve_x;
                            b.right -= resolve_x;
                        } else {
                            auto resolve_y = h;
                            if (a.transform->pos.y < b.transform->pos.y) {
                                resolve_y = -resolve_y;
                            }
                            b.transform->pos.y -= resolve_y;
                            b.left -= resolve_y;
                            b.right -= resolve_y;
                        }
                    }

                    // Post-collide
                    if (auto b_script = entities.get_component<component::script*>(b.eid)) {
                        engine.call_script("systems.physics", "post_collide", "actors."+b_script->name, b.eid, a.eid);
                        if (!entities.exists(obj.eid)) {
                            break;
                        }
                        if (!entities.exists(obj2->eid)) {
                            continue;
                        }
                    }
                    if (auto a_script = entities.get_component<component::script*>(a.eid)) {
                        engine.call_script("systems.physics", "post_collide", "actors."+a_script->name, a.eid, b.eid);
                        if (!entities.exists(obj.eid)) {
                            break;
                        }
                    }
                }
            }

            current.push_back(&obj);
        }
    }

    time -= delta * n_steps;
}
