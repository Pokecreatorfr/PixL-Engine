#pragma once
#include <const/Config.hpp>
#include <Graphics.hpp>

struct tile_animation
{
    int tile_index;
    std::vector<int> frames;
    int modulo;
};


struct tileset
{
    int uid;
    int tileset_width;
    int tileset_height;
    const image_ressource* ressource;
    std::vector<tile_animation> animations;
};