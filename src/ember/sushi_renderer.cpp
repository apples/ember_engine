#include "sushi_renderer.hpp"

#include <glm/gtc/matrix_inverse.hpp>

#include <cmath>

namespace ember {

sushi_renderer::sushi_renderer(
    const glm::vec2& display_area,
    shaders::basic_shader_program& program,
    shaders::msdf_shader_program& msdf_shader,
    cache<msdf_font>& font_cache,
    cache<sushi::mesh_group>& mesh_cache,
    cache<sushi::texture_2d>& texture_cache)
    : display_area(display_area),
      program(&program),
      msdf_shader(&msdf_shader),
      font_cache(&font_cache),
      mesh_cache(&mesh_cache),
      texture_cache(&texture_cache) {
    sushi::mesh_group_builder mb;
    mb.enable(sushi::attrib_location::POSITION);
    mb.enable(sushi::attrib_location::TEXCOORD);
    mb.enable(sushi::attrib_location::NORMAL);

    mb.mesh("");

    auto left = 0.f;
    auto right = 1.f;
    auto bottom = 0.f;
    auto top = 1.f;

    auto bottomleft = mb.vertex()
        .position({left, bottom, 0})
        .texcoord({0, 1})
        .normal({0, 0, 1})
        .get();

    auto topleft = mb.vertex()
        .position({left, top, 0})
        .texcoord({0, 0})
        .normal({0, 0, 1})
        .get();

    auto bottomright = mb.vertex()
        .position({right, bottom, 0})
        .texcoord({1, 1})
        .normal({0, 0, 1})
        .get();

    auto topright = mb.vertex()
        .position({right, top, 0})
        .texcoord({1, 0})
        .normal({0, 0, 1})
        .get();
    
    mb.tri(bottomleft, topleft, topright);
    mb.tri(topright, bottomright, bottomleft);

    rectangle_mesh = mb.get();
}

void sushi_renderer::begin() {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClear(GL_DEPTH_BUFFER_BIT);
}

void sushi_renderer::end() {
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

void sushi_renderer::draw_rectangle(const std::string& texture, const glm::vec4& color, glm::vec2 position, glm::vec2 size) {
    auto proj = glm::ortho(0.f, display_area.x, 0.f, display_area.y, 10.f, -10.f);
    auto model_mat = glm::mat4(1.f);

    model_mat = glm::translate(model_mat, glm::vec3(position, 0.f));
    model_mat = glm::scale(model_mat, glm::vec3(size, 1.f));

    program->bind();
    program->set_cam_forward({0.0, 0.0, -1.0});
    program->set_normal_mat(glm::inverseTranspose(model_mat));
    program->set_MVP(proj * model_mat);
    program->set_uvmat(glm::mat3(1.f));
    program->set_tint(color);
    program->set_hue(0);
    program->set_saturation(1);
    program->set_animated(false);

    sushi::set_texture(0, *texture_cache->get(texture));
    sushi::draw_mesh(rectangle_mesh);
}

void sushi_renderer::draw_model(const std::string& mesh, const std::string& texture, glm::vec2 position, glm::vec2 size, glm::mat4 model_mat) {
    auto half_size = size * 0.5f;

    auto bottom_left = -(position / half_size + glm::vec2{1.f, 1.f});
    auto top_right = (display_area - position) / half_size - glm::vec2{1.f, 1.f};

    auto proj = glm::ortho(bottom_left.x, top_right.x, bottom_left.y, top_right.y, -10.f, 10.f);
    auto view = glm::mat4(1.f);
    view = glm::rotate(view, glm::radians(36.87f), glm::vec3{1, 0, 0});
    view = glm::rotate(view, glm::radians(45.f), glm::vec3{0, 1, 0});

    program->bind();
    program->set_cam_forward({0.0, 0.0, -1.0});
    program->set_normal_mat(glm::inverseTranspose(view * model_mat));
    program->set_MVP(proj * view * model_mat);
    program->set_uvmat(glm::mat3(1.f));
    program->set_tint({1, 1, 1, 1});
    program->set_hue(0);
    program->set_saturation(1);

    sushi::set_texture(0, *texture_cache->get(texture));

    glEnable(GL_DEPTH_TEST);
    sushi::draw_mesh(*mesh_cache->get(mesh));
    glDisable(GL_DEPTH_TEST);
}

float sushi_renderer::get_text_width(const std::string& text, const std::string& fontname) {
    auto font = font_cache->get(fontname);
    auto width = 0.f;

    for (auto c : text) {
        auto& glyph = font->get_glyph(c);
        width += glyph.advance;
    }

    return width;
}

void sushi_renderer::draw_text(const std::string& text, const std::string& fontname, const glm::vec4& color, glm::vec2 position, float size) {
    auto font = font_cache->get(fontname);
    auto proj = glm::ortho(0.f, display_area.x, 0.f, display_area.y, -1.f, 1.f);
    auto model = glm::scale(glm::translate(glm::mat4(1.f), glm::vec3(position, 0.f)), glm::vec3{size, size, 1.f});

    msdf_shader->bind();
    msdf_shader->set_msdf(0);
    msdf_shader->set_pxRange(4.f);
    msdf_shader->set_fgColor(color);

    for (auto c : text) {
        auto& glyph = font->get_glyph(c);

        msdf_shader->set_MVP(proj * model);
        msdf_shader->set_texSize({glyph.texture.width, glyph.texture.height});

        sushi::set_texture(0, glyph.texture);
        sushi::draw_mesh(glyph.mesh);

        model = glm::translate(model, glm::vec3{glyph.advance, 0.f, 0.f});
    }
}

} // namespace ember
