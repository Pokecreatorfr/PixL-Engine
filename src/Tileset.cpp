#include <Tileset.hpp>

Tileset::Tileset( tileset const* tileset, camera* camera)
{
    this->uid = tileset->uid;
    this->tileset_width = tileset->tileset_width;
    this->tileset_height = tileset->tileset_height;

    this->camera = camera;

    this->texture = Load_Texture(*tileset->ressource, camera->renderer);

    for(int i = 0; i < tileset->tileset_height; i++)
    {
        for(int j = 0; j < tileset->tileset_width; j++)
        {
            this->tile_rects[i * tileset->tileset_width + j].x = j * TILE_SIZE;
            this->tile_rects[i * tileset->tileset_width + j].y = i * TILE_SIZE;
            this->tile_rects[i * tileset->tileset_width + j].w = TILE_SIZE;
            this->tile_rects[i * tileset->tileset_width + j].h = TILE_SIZE;
        }
    }
}

inline Tileset::~Tileset()
{
    SDL_DestroyTexture(this->texture);
}

inline int Tileset::get_uid()
{
    return this->uid;
}

inline int Tileset::get_tileset_width()
{
    return this->tileset_width;
}

inline int Tileset::get_tileset_height()
{
    return this->tileset_height;
}

void Tileset::draw_tile(int tile_index, int x, int y , int w, int h)
{
    SDL_Rect dest = {x, y, w, h};
    SDL_RenderCopy(this->camera->renderer, this->texture, &this->tile_rects[tile_index], &dest);
}

int Tileset::get_tile_index(int tile_index)
{
    // TODO 
    return tile_index;
}