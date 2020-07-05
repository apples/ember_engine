#pragma once

#include "camera.hpp"
#include "engine.hpp"
#include "render_queue.hpp"

#include <sushi/sushi.hpp>

#include <memory>
#include <optional>

namespace ember::ez3d {

/** Model used for rendering */
struct model {
    std::shared_ptr<sushi::mesh_group> mesh;
    std::shared_ptr<sushi::texture_2d> texture;
    std::shared_ptr<sushi::skeleton> skeleton;
    std::optional<int> anim_index;
    float anim_time;
};

class renderer {
public:
    void begin(engine* eng, const camera::perspective& cam);

    void add(const model& model, const sushi::transform& transform);

    void finish();

private:
    render_queue renderq;
};

} // namespace ember::ez3d
