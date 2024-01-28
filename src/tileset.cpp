#include <tileset.hpp>

Tileset::Tileset(SDL_Renderer* renderer,const tileset* tileset,overworld_vars* overworld_struct)
{
    this->texture = Load_Texture(*tileset->ressource, renderer);
    this->renderer = renderer;
    this->animations = std::vector<tile_animation>();
    this->uid = tileset->uid;
    for(int i = 0 ; i < tileset->tileset_height ; i++)
    {
        for(int j = 0 ; j < tileset->tileset_width ; j++)
        {
            SDL_Rect tile;
            tile.x = j * TILE_SIZE;
            tile.y = i * TILE_SIZE;
            tile.w = TILE_SIZE;
            tile.h = TILE_SIZE;
            this->tiles.push_back(tile);
        }
    }
    this->overworld_struct = overworld_struct;
}

Tileset::~Tileset()
{
    SDL_DestroyTexture(this->texture);
}

void Tileset::get_tile(int tile_index, SDL_Rect* tile)
{
    *tile = this->tiles[tile_index];
}

SDL_Texture* Tileset::get_texture()
{
    return this->texture;
}