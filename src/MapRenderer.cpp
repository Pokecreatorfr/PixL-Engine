#include <MapRenderer.hpp>

MapRenderer::MapRenderer(const map_struct* map, Tileset* ts0, Tileset* ts1, Tileset* ts2)
{
    this->Camera = Camera::GetInstance();

    this->map = map;

    this->tilemap_renderers.push_back(new TilemapRenderer(&map->tile_layer_0 ,map->map_pos_x, map->map_pos_y, map->width , map->height , ts0));
    this->tilemap_renderers.push_back(new TilemapRenderer(&map->tile_layer_1 ,map->map_pos_x, map->map_pos_y, map->width , map->height , ts1));
    this->tilemap_renderers.push_back(new TilemapRenderer(&map->tile_layer_2 ,map->map_pos_x, map->map_pos_y, map->width , map->height , ts2));
}

MapRenderer::~MapRenderer()
{
    for(int i = 0; i < this->tilemap_renderers.size(); i++)
    {
        delete this->tilemap_renderers[i];
    }
}

int MapRenderer::get_uid()
{
    return this->map->uid;
}

int MapRenderer::get_width()
{
    return this->map->width;
}

int MapRenderer::get_height()
{
    return this->map->height;
}

int MapRenderer::get_map_pos_x()
{
    return this->map->map_pos_x;
}

int MapRenderer::get_map_pos_y()
{
    return this->map->map_pos_y;
}

void MapRenderer::draw_layer0()
{
    this->tilemap_renderers[0]->draw();
}

void MapRenderer::draw_layer1()
{
    this->tilemap_renderers[1]->draw();
}

void MapRenderer::draw_layer2()
{
    this->tilemap_renderers[2]->draw();
}

std::vector<int> MapRenderer::get_tilesets_uid()
{
    std::vector<int> t;
    t.push_back(map->tile_layer_0.tileset->uid);
    if (map->tile_layer_1.tileset->uid != map->tile_layer_0.tileset->uid)
    {
        t.push_back(map->tile_layer_1.tileset->uid);
    }
    if (map->tile_layer_2.tileset->uid != map->tile_layer_0.tileset->uid && map->tile_layer_2.tileset->uid != map->tile_layer_1.tileset->uid)
    {
        t.push_back(map->tile_layer_2.tileset->uid);
    }

    return t;
}