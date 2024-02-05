#include "map_renderer.hpp"

TilemapRenderer::TilemapRenderer(map_struct *map, int layer)
{
    std::vector<int> tile_layer;
    switch (layer)
    {
        case MAP_LAYER_0:
            tile_layer = map->tile_layer_0;
            break;
        case MAP_LAYER_1:
            tile_layer = map->tile_layer_1;
            break;
        case MAP_LAYER_2:
            tile_layer = map->tile_layer_2;
            break;
    }
    for (int i = 0; i < map->height; i++)
	{
		for (int j = 0; j < map->width; j++)
		{
            int tile_index = tile_layer[0];
			if (tile_index != 0)
			{
				coord_2d position;
				position.x = (j + map->map_pos_x ) * 32;
				position.y = (i + map->map_pos_y ) * 32;
				coord_2d size;
				size.x = TILE_SIZE;
				size.y = TILE_SIZE;
				Tile tile(position, size, tile_index);
				tiles.push_back(tile);
			}
		}
	}
}

void TilemapRenderer::Draw(Tileset* tileset,Camera camera,SDL_Renderer* renderer)
{
    	for (int i = 0; i < tiles.size(); i++)
	{
		SDL_Rect rect;
		// render tiles with camera offset and zoom factor ; code foru
		rect.x = (tiles[i].position.x - camera.position.x) * camera.zoom + camera.size.x / 2;
		rect.y = (tiles[i].position.y - camera.position.y) * camera.zoom + camera.size.y / 2;
		rect.w = (tiles[i].size.x)* camera.zoom;
		rect.h = (tiles[i].size.y)* camera.zoom;
		SDL_RenderCopy(renderer, renderer, &tileset->tiles[tiles[i].tile_index-1], &rect);
	}
}