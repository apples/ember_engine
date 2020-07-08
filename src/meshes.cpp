#include "meshes.hpp"

auto get_sprite_mesh() -> sushi::mesh_group {
    sushi::mesh_group_builder mb;
    mb.enable(sushi::attrib_location::POSITION);
    mb.enable(sushi::attrib_location::TEXCOORD);
    mb.enable(sushi::attrib_location::NORMAL);

    mb.mesh("sprite");

    auto left = -0.5f;
    auto right = 0.5f;
    auto bottom = -0.5f;
    auto top = 0.5f;

    auto bottomleft = mb.vertex().position({left, bottom, 0}).texcoord({0, 1}).normal({0, 1, 0}).get();
    auto topleft = mb.vertex().position({left, top, 0}).texcoord({0, 0}).normal({0, 1, 0}).get();
    auto bottomright = mb.vertex().position({right, bottom, 0}).texcoord({1, 1}).normal({0, 1, 0}).get();
    auto topright = mb.vertex().position({right, top, 0}).texcoord({1, 0}).normal({0, 1, 0}).get();

    mb.tri(bottomleft, topleft, topright);
    mb.tri(topright, bottomright, bottomleft);

    return mb.get();
}

auto get_tilemap_mesh() -> sushi::mesh_group {
    sushi::mesh_group_builder mb;
    mb.enable(sushi::attrib_location::POSITION);
    mb.enable(sushi::attrib_location::TEXCOORD);
    mb.enable(sushi::attrib_location::NORMAL);

    mb.mesh("tilemap");

    for (int r = 0; r < 32; ++r) {
        for (int c = 0; c < 32; ++c) {
            auto left = -16.f + c;
            auto right = left + 1;
            auto top = 16.f - r;
            auto bottom = top - 1;

            float uvleft, uvright, uvtop, uvbottom;

            if (r == 0 || c == 0 || c == 31) {
                uvleft = 0.25;
                uvright = 0.5;
                uvtop = 0.75;
                uvbottom = 1;
            } else {
                uvleft = 0;
                uvright = 0.25;
                uvtop = 0.75;
                uvbottom = 1;
            }

            auto bottomleft = mb.vertex()
                .position({left, bottom, 0})
                .texcoord({uvleft, uvbottom})
                .normal({0, 1, 0})
                .get();
            auto topleft = mb.vertex()
                .position({left, top, 0})
                .texcoord({uvleft, uvtop})
                .normal({0, 1, 0})
                .get();
            auto bottomright = mb.vertex()
                .position({right, bottom, 0})
                .texcoord({uvright, uvbottom})
                .normal({0, 1, 0})
                .get();
            auto topright = mb.vertex()
                .position({right, top, 0})
                .texcoord({uvright, uvtop})
                .normal({0, 1, 0})
                .get();

            mb.tri(bottomleft, topleft, topright);
            mb.tri(topright, bottomright, bottomleft);
        }
    }

    return mb.get();
}