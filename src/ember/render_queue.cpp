#include "render_queue.hpp"

#include <glm/gtc/matrix_inverse.hpp>

namespace ember {

/** Prepares the queue */
void render_queue::prepare(
    shaders::basic_shader_program& shader, const glm::mat4& proj_matrix, const glm::mat4& view_matrix) {
    program_basic = &shader;
    proj = proj_matrix;
    view = view_matrix;
    frustum = {proj * view};
}

/** Adds an item to the queue if it is within the view frustum */
void render_queue::add_render_item(const render_item& item) {
    if (frustum.contains(item.transform.pos, 2)) {
        queue.push_back(item);
    }
}

/** Renders all queued items */
void render_queue::render_items() {
    auto forward = -glm::vec3(glm::row(view, 2));

    std::sort(begin(queue), end(queue), [&](const render_item& a, const render_item& b) {
        auto dist_a = glm::dot(a.transform.pos, forward);
        auto dist_b = glm::dot(b.transform.pos, forward);

        return dist_a > dist_b;
    });

    for (const auto& item : queue) {
        auto modelmat = to_mat4(item.transform);

        if (item.attached_to) {
            auto& pose = item.attached_to->pose;
            auto bone_idx = item.attached_to->bone_idx;
            auto bt = pose.get_bone_transform(bone_idx);
            modelmat = modelmat * bt;
        }

        program_basic->set_tint(item.tint);
        program_basic->set_normal_mat(glm::inverseTranspose(view * modelmat));
        program_basic->set_MVP(proj * view * modelmat);

        sushi::set_texture(0, *item.texture);

        auto draw = [&] {
            if (item.pose) {
                sushi::draw_mesh(*item.mesh, *item.pose);
            } else {
                sushi::draw_mesh(*item.mesh);
            }
        };

        // Depth pre-pass
        glColorMask(false, false, false, false);
        draw();

        glColorMask(true, true, true, true);
        draw();
    }

    queue.clear();
}

} // namespace ember
