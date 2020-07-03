#include "font.hpp"

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

msdfgen::FreetypeHandle* font_init() {
    static const auto ft = msdfgen::initializeFreetype();
    return ft;
}

} //static

namespace ember {

msdf_font::msdf_font(const std::string& fontname) {
    auto ft = font_init();

    if (!ft) {
        throw std::runtime_error("Failed to initialize FreeType.");
    }

    font = decltype(font)(msdfgen::loadFont(ft, fontname.c_str()));

    if (!font) {
        throw std::runtime_error("Failed to load font "+fontname+".");
    }
}

const msdf_font::glyph& msdf_font::get_glyph(int unicode) const {
    auto iter = glyphs.find(unicode);

    if (iter == end(glyphs)) {
        msdfgen::Shape shape;
        double advance;

        if (!msdfgen::loadGlyph(shape, font.get(), unicode, &advance)) {
            return glyphs.at(0);
        }

        shape.normalize();
        msdfgen::edgeColoringSimple(shape, 3.0);

        double left=0, bottom=0, right=0, top=0;
        shape.bounds(left, bottom, right, top);

        left -= 1;
        bottom -= 1;
        right += 1;
        top += 1;

        auto width = int(right - left + 1);
        auto height = int(top - bottom + 1);

        msdfgen::Bitmap<msdfgen::FloatRGB> msdf(width, height);
        msdfgen::generateMSDF(msdf, shape, 4.0, 1.0, msdfgen::Vector2(-left, -bottom));

        std::vector<unsigned char> pixels;
        pixels.reserve(4*msdf.width()*msdf.height());
        for (int y = 0; y < msdf.height(); ++y) {
            for (int x = 0; x < msdf.width(); ++x) {
                pixels.push_back(msdfgen::clamp(msdf(x, y).r * 256.f, 255.f));
                pixels.push_back(msdfgen::clamp(msdf(x, y).g * 256.f, 255.f));
                pixels.push_back(msdfgen::clamp(msdf(x, y).b * 256.f, 255.f));
                pixels.push_back(255);
            }
        }

        double em;
        msdfgen::getFontScale(em, font.get());

        left /= em;
        right /= em;
        bottom /= em;
        top /= em;
        advance /= em;

        auto g = glyph{};

        auto mb = sushi::mesh_group_builder{};
        mb.enable(sushi::attrib_location::POSITION);
        mb.enable(sushi::attrib_location::TEXCOORD);
        mb.enable(sushi::attrib_location::NORMAL);

        mb.mesh("");

        auto bottomleft = mb.vertex()
            .position({left, bottom, 0})
            .texcoord({0, 0})
            .normal({0, 0, 1})
            .get();

        auto topleft = mb.vertex()
            .position({left, top, 0})
            .texcoord({0, 1})
            .normal({0, 0, 1})
            .get();

        auto bottomright = mb.vertex()
            .position({right, bottom, 0})
            .texcoord({1, 0})
            .normal({0, 0, 1})
            .get();

        auto topright = mb.vertex()
            .position({right, top, 0})
            .texcoord({1, 1})
            .normal({0, 0, 1})
            .get();
        
        mb.tri(bottomleft, topleft, topright);
        mb.tri(topright, bottomright, bottomleft);

        g.mesh = mb.get();

        g.texture.handle = sushi::make_unique_texture();
        g.texture.width = width;
        g.texture.height = height;

        glBindTexture(GL_TEXTURE_2D, g.texture.handle.get());
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, msdf.width(), msdf.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, &pixels[0]);

        g.advance = advance;

        iter = glyphs.insert({unicode, std::move(g)}).first;
    }

    return iter->second;
}

} // namespace ember
