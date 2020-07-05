#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <sushi/sushi.hpp>

#include <memory>
#include <optional>

namespace ember::camera {

/** Perspective projection camera */
struct perspective {
    float fov_y = 90.f;
    float aspect_ratio = 4.f/3.f; /** fov_x/fov_y */
    float near = 0.05f;
    float far = 100.f;
    glm::vec3 pos = {0, 0, 0};
    glm::quat rot = {1, 0, 0, 0}; /** Orientation from forward: -Z, up: Y */
};

auto get_proj(const perspective& cam) -> glm::mat4;

auto get_view(const perspective& cam) -> glm::mat4;

auto get_forward(const perspective& cam) -> glm::vec3;

void look_at(perspective& cam, const glm::vec3& target, const glm::vec3& up);

/** Orthographic projection camera */
struct orthographic {
    float height = 2.f;
    float aspect_ratio = 4.f/3.f; /** width/height */
    float near = 0.f;
    float far = 100.f;
    glm::vec3 pos = {0, 0, 0};
    glm::quat rot = {1, 0, 0, 0}; /** Orientation from forward: -Z, up: Y */
};

auto get_proj(const orthographic& cam) -> glm::mat4;

auto get_view(const orthographic& cam) -> glm::mat4;

auto get_forward(const orthographic& cam) -> glm::vec3;

void look_at(orthographic& cam, const glm::vec3& target, const glm::vec3& up);

} // namespace ember::camera
