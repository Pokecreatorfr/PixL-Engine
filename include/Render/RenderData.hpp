#pragma once
#include <cstdint>
#include <string>
#include <vector>

struct sprite_sheet
{
    std::string texture_path;
    int sprite_height;
    int sprite_width;
    int texture_height;
    int texture_width;
};

struct sprite_render_data
{
    int x;
    int y;
    float rotation;
    int sprite_sheet_index;
    uint16_t priority; // 0 is the highest priority
};

struct tileset
{
    std::string texture_path;
    int tile_height;
    int tile_width;
    int texture_height;
    int texture_width;
};

struct tile_render_data
{
    int x;
    int y;
    int tileset_index;
    int tile_index;
};

struct tilemap_render_data
{
    std::vector<tile_render_data> tiles;
    uint16_t priority; // 0 is the highest priority
};

struct RenderData
{
    std::vector<sprite_render_data> sprites;
    std::vector<tilemap_render_data> tilemaps;
};