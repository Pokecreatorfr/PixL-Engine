#include <MapRenderer.hpp>

MapRenderer::MapRenderer(camera *cam , map_struct map)
{
    this->Camera = cam;

    this->map.uid = map.uid;
    this->map.width = map.width;
    this->map.height = map.height;
    this->map.map_pos_x = map.map_pos_x;
    this->map.map_pos_y = map.map_pos_y;
    this->map.tile_layer_0 = map.tile_layer_0;
    this->map.tile_layer_1 = map.tile_layer_1;
    this->map.tile_layer_2 = map.tile_layer_2;
    this->map.collision_layer = map.collision_layer;
    this->map.entity_layer = map.entity_layer;

    this->tilesets.push_back(new Tileset(map.tile_layer_0.tileset, cam));
    this->tilesets.push_back(new Tileset(map.tile_layer_1.tileset, cam));
    this->tilesets.push_back(new Tileset(map.tile_layer_2.tileset, cam));
}

int MapRenderer::get_uid()
{
    return this->map.uid;
}

int MapRenderer::get_width()
{
    return this->map.width;
}

int MapRenderer::get_height()
{
    return this->map.height;
}

int MapRenderer::get_map_pos_x()
{
    return this->map.map_pos_x;
}

int MapRenderer::get_map_pos_y()
{
    return this->map.map_pos_y;
}

void MapRenderer::draw_layer0()
{
    draw(this->map.tile_layer_0, this->tilesets[0]);
}

void MapRenderer::draw_layer1()
{
    draw(this->map.tile_layer_1, this->tilesets[1]);
}

void MapRenderer::draw_layer2()
{
    draw(this->map.tile_layer_2, this->tilesets[2]);
}

std::vector<int> MapRenderer::get_tilesets_uid()
{
    std::vector<int> t;
    t.push_back(map.tile_layer_0.tileset->uid);
    if (map.tile_layer_1.tileset->uid != map.tile_layer_0.tileset->uid)
    {
        t.push_back(map.tile_layer_1.tileset->uid);
    }
    if (map.tile_layer_2.tileset->uid != map.tile_layer_0.tileset->uid && map.tile_layer_2.tileset->uid != map.tile_layer_1.tileset->uid)
    {
        t.push_back(map.tile_layer_2.tileset->uid);
    }

    return t;
}

void MapRenderer::draw(tiles_layer tl , Tileset* ts)
{
    for(int i = 0; i < this->map.width; i++)
    {
        for(int j = 0; j < this->map.height; j++)
        {
            if (tl.tiles[i + j * this->map.width] != -1)
            {
                ts->draw_tile(
                    tl.tiles[i + j * this->map.width],
                    ((map.map_pos_x + i) * TILE_SIZE - Camera->position.x) * Camera->zoom + Camera->size.x / 2,
                    ((map.map_pos_y + j) * TILE_SIZE - Camera->position.y) * Camera->zoom + Camera->size.y / 2,
                    TILE_SIZE * Camera->zoom,
                    TILE_SIZE* Camera->zoom
                    );
            }
        }
    }
}
