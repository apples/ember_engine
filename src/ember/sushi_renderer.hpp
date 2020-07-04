#pragma once

#include "gui.hpp"
#include "font.hpp"
#include "shaders.hpp"
#include "resource_cache.hpp"

#include <sushi/sushi.hpp>

#include <string>

namespace ember {

class sushi_renderer final : public gui::render_context {
public:
    template <typename T>
    using cache = resource_cache<T, std::string>;

    sushi_renderer() = default;
    sushi_renderer(
        const glm::vec2& display_area,
        shaders::basic_shader_program& program,
        shaders::msdf_shader_program& msdf_shader,
        cache<msdf_font>& font_cache,
        cache<sushi::mesh_group>& mesh_cache,
        cache<sushi::texture_2d>& texture_cache);

    virtual void begin() override;
    virtual void end() override;
    virtual void draw_rectangle(const std::string& texture, const glm::vec4& color, glm::vec2 position, glm::vec2 size) override;
    virtual void draw_model(const std::string& mesh, const std::string& texture, glm::vec2 position, glm::vec2 size, glm::mat4 model_mat) override;
    virtual void draw_text(const std::string& text, const std::string& font, const glm::vec4& color, glm::vec2 position, float size) override;
    virtual float get_text_width(const std::string& text, const std::string& font) override;

private:
    glm::vec2 display_area;
    shaders::basic_shader_program* program;
    shaders::msdf_shader_program* msdf_shader;
    cache<msdf_font>* font_cache;
    cache<sushi::mesh_group>* mesh_cache;
    cache<sushi::texture_2d>* texture_cache;
    sushi::mesh_group rectangle_mesh;
};

} // namespace ember
