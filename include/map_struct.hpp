#pragma once
#include <vector>
#include <Graphics.hpp>
#include <tileset.hpp>


struct map_struct
{
    int width;
    int height;
    int map_pos_x;
    int map_pos_y;
    std::vector<int> tile_layer_0;
    std::vector<int> tile_layer_1;
    std::vector<int> tile_layer_2;
    std::vector<int> collision_layer;
    std::vector<int> entity_layer;
    const tileset* tileset;
};