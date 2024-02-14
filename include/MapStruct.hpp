#pragma once
#include <vector>
#include <Graphics.hpp>
#include <tileset.hpp>

struct tiles_layer
{
    const tileset* tileset;
    const int tiles[];
};

struct map_struct
{
    int width;
    int height;
    int map_pos_x;
    int map_pos_y;
    const tiles_layer tile_layer_0;
    const tiles_layer tile_layer_1;
    const tiles_layer tile_layer_2;
    std::vector<int> collision_layer;
    std::vector<int> entity_layer;
};