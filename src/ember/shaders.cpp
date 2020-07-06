#include "shaders.hpp"

namespace ember::shaders {

basic_shader_program::basic_shader_program(const std::string& vert, const std::string& frag) :
    sushi::shader_base({
        {sushi::shader_type::VERTEX, vert},
        {sushi::shader_type::FRAGMENT, frag},
    })
{
    bind();

    uniforms.MVP = get_uniform_location("MVP");
    uniforms.normal_mat = get_uniform_location("normal_mat");
    uniforms.uvmat = get_uniform_location("uvmat");
    uniforms.s_texture = get_uniform_location("s_texture");
    uniforms.cam_forward = get_uniform_location("cam_forward");
    uniforms.tint = get_uniform_location("tint");
    uniforms.hue = get_uniform_location("hue");
    uniforms.saturation = get_uniform_location("saturation");
    uniforms.animated = get_uniform_location("Animated");
    uniforms.bones = get_uniform_location("Bones");
}

void basic_shader_program::set_MVP(const glm::mat4& mat) {
    sushi::set_current_program_uniform(uniforms.MVP, mat);
}

void basic_shader_program::set_normal_mat(const glm::mat4& mat) {
    sushi::set_current_program_uniform(uniforms.normal_mat, mat);
}

void basic_shader_program::set_uvmat(const glm::mat3& mat) {
    sushi::set_current_program_uniform(uniforms.uvmat, mat);
}

void basic_shader_program::set_s_texture(GLint i) {
    sushi::set_current_program_uniform(uniforms.s_texture, i);
}

void basic_shader_program::set_cam_forward(const glm::vec3& vec) {
    sushi::set_current_program_uniform(uniforms.cam_forward, vec);
}

void basic_shader_program::set_tint(const glm::vec4& v) {
    sushi::set_current_program_uniform(uniforms.tint, v);
}

void basic_shader_program::set_hue(float f) {
    sushi::set_current_program_uniform(uniforms.hue, f);
}

void basic_shader_program::set_saturation(float f) {
    sushi::set_current_program_uniform(uniforms.saturation, f);
}

void basic_shader_program::set_animated(bool b) {
    sushi::set_current_program_uniform(uniforms.animated, int(b));
}

void basic_shader_program::set_bones(const glm::mat4* ms, std::size_t n) {
    sushi::set_current_program_uniform(uniforms.bones, ms, n);
}

msdf_shader_program::msdf_shader_program(const std::string& vertfile, const std::string& fragfile) :
    sushi::shader_base({
        {sushi::shader_type::VERTEX, vertfile},
        {sushi::shader_type::FRAGMENT, fragfile}
    })
{
    bind();
    uniforms.MVP = get_uniform_location("MVP");
    uniforms.normal_mat = get_uniform_location("normal_mat");
    uniforms.msdf = get_uniform_location("msdf");
    uniforms.pxRange = get_uniform_location("pxRange");
    uniforms.texSize = get_uniform_location("texSize");
    uniforms.fgColor = get_uniform_location("fgColor");
}

void msdf_shader_program::set_MVP(const glm::mat4& mat) {
    sushi::set_current_program_uniform(uniforms.MVP, mat);
}

void msdf_shader_program::set_normal_mat(const glm::mat4& mat) {
    sushi::set_current_program_uniform(uniforms.normal_mat, mat);
}

void msdf_shader_program::set_msdf(int slot) {
    sushi::set_current_program_uniform(uniforms.msdf, slot);
}

void msdf_shader_program::set_pxRange(float f) {
    sushi::set_current_program_uniform(uniforms.pxRange, f);
}

void msdf_shader_program::set_texSize(const glm::vec2& vec) {
    sushi::set_current_program_uniform(uniforms.texSize, vec);
}

void msdf_shader_program::set_fgColor(const glm::vec4& vec) {
    sushi::set_current_program_uniform(uniforms.fgColor, vec);
}

} // namespace ember::shaders
