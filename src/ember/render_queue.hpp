#pragma once

#include "shaders.hpp"

#include <sushi/sushi.hpp>
#include <glm/glm.hpp>

#include <memory>
#include <optional>
#include <vector>

namespace ember {

class render_queue {
public:
    struct item {
        struct attachment {
            sushi::pose pose;
            int bone_idx;
        };

        sushi::transform transform;
        glm::vec4 tint = {1, 1, 1, 1};
        std::shared_ptr<sushi::mesh_group> mesh;
        std::shared_ptr<sushi::texture_2d> texture;
        std::optional<sushi::pose> pose;
        std::optional<attachment> attached_to;
    };

    /** Prepares the queue */
    void prepare(
        shaders::basic_shader_program& shader,
        const glm::mat4& proj_matrix,
        const glm::mat4& view_matrix);

    /** Adds an item to the queue if it is within the view frustum */
    void add(const item& item);

    /** Renders all queued items */
    void render_items();

private:
    std::vector<item> queue;
    shaders::basic_shader_program* basic_shader;
    glm::mat4 proj;
    glm::mat4 view;
    sushi::frustum frustum;
};

} // namespace ember
