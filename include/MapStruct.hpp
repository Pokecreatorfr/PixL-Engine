#pragma once
#include <vector>
#include <Graphics.hpp>
#include <Tileset.hpp>

struct tiles_layer
{
    const tileset* tileset;
    std::vector<int> tiles;
};

struct map_struct
{
    int uid;
    int width;
    int height;
    int map_pos_x;
    int map_pos_y;
    tiles_layer tile_layer_0;
    tiles_layer tile_layer_1;
    tiles_layer tile_layer_2;
    std::vector<int> collision_layer;
    std::vector<int> entity_layer;
};