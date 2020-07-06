#pragma once

#include <sushi/sushi.hpp>

namespace ember::shaders {

class basic_shader_program : public sushi::shader_base {
public:
    basic_shader_program() = default;

    basic_shader_program(const std::string& vert, const std::string& frag);

    void set_MVP(const glm::mat4& mat);
    void set_normal_mat(const glm::mat4& mat);
    void set_uvmat(const glm::mat3& mat);
    void set_s_texture(GLint i);
    void set_cam_forward(const glm::vec3& vec);
    void set_tint(const glm::vec4& v);
    void set_hue(float f);
    void set_saturation(float f);
    void set_animated(bool b);
    void set_bones(const glm::mat4* ms, std::size_t n);

private:
    struct {
        GLint MVP;
        GLint normal_mat;
        GLint uvmat;
        GLint s_texture;
        GLint cam_forward;
        GLint tint;
        GLint hue;
        GLint saturation;
        GLint animated;
        GLint bones;
    } uniforms;
};

class msdf_shader_program : public sushi::shader_base {
public:
    msdf_shader_program() = default;

    msdf_shader_program(const std::string& vertfile, const std::string& fragfile);

    void set_MVP(const glm::mat4& mat);
    void set_normal_mat(const glm::mat4& mat);
    void set_msdf(int slot);
    void set_pxRange(float f);
    void set_texSize(const glm::vec2& vec);
    void set_fgColor(const glm::vec4& vec);

private:
    struct {
        GLint MVP;
        GLint normal_mat;
        GLint msdf;
        GLint pxRange;
        GLint texSize;
        GLint fgColor;
    } uniforms;
};

} // namespace ember::shaders
