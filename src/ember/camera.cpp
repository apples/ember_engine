#include "camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace ember::camera {

// Perspective

auto get_proj(const perspective& cam) -> glm::mat4 {
    return glm::perspective(cam.fov_y, cam.aspect_ratio, cam.near, cam.far);
}

auto get_view(const perspective& cam) -> glm::mat4 {
    auto mat = glm::mat4(1.f);
    mat = glm::translate(mat, cam.pos);
    mat = glm::mat4_cast(cam.rot) * mat;
    return mat;
}

auto get_forward(const perspective& cam) -> glm::vec3 {
    return cam.rot * glm::vec3{0, 0, -1};
}

void look_at(perspective& cam, const glm::vec3& target, const glm::vec3& up) {
    auto forward = glm::normalize(target - cam.pos);

    if (glm::length(forward) == 0) {
        return;
    }

    cam.rot = glm::quatLookAt(forward, up);
}

// Orthographic

auto get_proj(const orthographic& cam) -> glm::mat4 {
    auto width = cam.height * cam.aspect_ratio;

    return glm::ortho(-width/2, width/2, -cam.height/2, cam.height/2, cam.near, cam.far);
}

auto get_view(const orthographic& cam) -> glm::mat4 {
    auto mat = glm::mat4(1.f);
    mat = glm::translate(mat, cam.pos);
    mat = glm::mat4_cast(cam.rot) * mat;
    return mat;
}

auto get_forward(const orthographic& cam) -> glm::vec3 {
    return cam.rot * glm::vec3{0, 0, -1};
}

void look_at(orthographic& cam, const glm::vec3& target, const glm::vec3& up) {
    auto forward = glm::normalize(target - cam.pos);

    if (glm::length(forward) == 0) {
        return;
    }

    cam.rot = glm::quatLookAt(forward, up);
}

} // namespace ember::camera
