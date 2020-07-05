#include "ez3d.hpp"

namespace ember::ez3d {

void renderer::begin(engine* eng, const camera::perspective& cam) {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    //glEnable(GL_SAMPLE_COVERAGE);
    //glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);

    eng->basic_shader.bind();
    eng->basic_shader.set_cam_forward(get_forward(cam));
    eng->basic_shader.set_tint({1, 1, 1, 1});
    eng->basic_shader.set_hue(0);
    eng->basic_shader.set_saturation(1);

    renderq.prepare(eng->basic_shader, get_proj(cam), get_view(cam));
}

void renderer::add(const model& model, const sushi::transform& transform) {
    if (model.mesh && model.texture) {
        // Pose

        std::optional<sushi::pose> pose;

        if (model.skeleton) {
            pose = sushi::get_pose(*model.skeleton, model.anim_index, model.anim_time, true);
        }

        // Render

        auto item = render_queue::item{};
        item.transform = transform;
        item.mesh = model.mesh;
        item.texture = model.texture;
        item.pose = pose;

        renderq.add(item);
    }
}

void renderer::finish() {
    renderq.render_items();
}

} // namespace ember::ez3d
