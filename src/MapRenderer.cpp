#include "MapRenderer.hpp"

MapRenderer::MapRenderer(const map_struct *map, Tileset *ts0, Tileset *ts1, Tileset *ts2)
{
    Camera = Camera::GetInstance();
    this->map = map;
    tilemap_renderers.push_back(new TilemapRenderer(&map->tile_layer_0 , map->map_pos_x, map->map_pos_y, map->width, map->height, ts0));
    tilemap_renderers.push_back(new TilemapRenderer(&map->tile_layer_1 , map->map_pos_x, map->map_pos_y, map->width, map->height, ts1));
    tilemap_renderers.push_back(new TilemapRenderer(&map->tile_layer_2 , map->map_pos_x, map->map_pos_y, map->width, map->height, ts2));
}

MapRenderer::~MapRenderer()
{
    for (int i = 0; i < tilemap_renderers.size(); i++)
    {
        delete tilemap_renderers[i];
    }
}

int MapRenderer::get_uid()
{
    return map->uid;
}

int MapRenderer::get_width()
{
    return map->width;
}

int MapRenderer::get_height()
{
    return map->height;
}

int MapRenderer::get_map_pos_x()
{
    return map->map_pos_x;
}

int MapRenderer::get_map_pos_y()
{
    return map->map_pos_y;
}

void MapRenderer::draw_layer0()
{
    tilemap_renderers[0]->draw();
}

void MapRenderer::draw_layer1()
{
    tilemap_renderers[1]->draw();
}

void MapRenderer::draw_layer2()
{
    tilemap_renderers[2]->draw();
}

std::vector<int> MapRenderer::get_tilesets_uid()
{
    std::vector<int> tilesets_uid;
    tilesets_uid.push_back(tilemap_renderers[0]->get_tileset()->get_uid());
    tilesets_uid.push_back(tilemap_renderers[1]->get_tileset()->get_uid());
    tilesets_uid.push_back(tilemap_renderers[2]->get_tileset()->get_uid());
    return tilesets_uid;
}
