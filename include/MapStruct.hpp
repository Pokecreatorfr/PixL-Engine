#pragma once
#include <vector>
#include <Graphics.hpp>
#include <Tileset.hpp>

struct tiles_layer
{
    const tileset* tileset;
    const std::vector<int> tiles;
};

struct map_struct
{
    int uid;
    int width;
    int height;
    int map_pos_x;
    int map_pos_y;
    const tiles_layer tile_layer_0;
    const tiles_layer tile_layer_1;
    const tiles_layer tile_layer_2;
    const std::vector<int> collision_layer;
    const std::vector<int> entity_layer;
};