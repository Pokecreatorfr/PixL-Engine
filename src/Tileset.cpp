#include <Tileset.hpp>

Tileset::Tileset(const tileset* tileset)
{
    this->uid = tileset->uid;
    this->tileset_width = tileset->tileset_width;
    this->tileset_height = tileset->tileset_height;

    this->Camera = Camera::GetInstance();

    this->texture = Load_Texture(*tileset->ressource, this->Camera->GetRenderer());
    
    for(int i = 0; i < tileset->tileset_height; i++)
    {
        for(int j = 0; j < tileset->tileset_width; j++)
        {
            SDL_Rect* rect = new SDL_Rect();
            rect->x = j * TILE_SIZE;
            rect->y = i * TILE_SIZE;
            rect->w = TILE_SIZE;
            rect->h = TILE_SIZE;
            this->tile_rects.push_back(rect);
        }
    }
    
}

Tileset::~Tileset()
{
    SDL_DestroyTexture(this->texture);
    for(int i = 0; i < this->tile_rects.size(); i++)
    {
        delete this->tile_rects[i];
    }
}

int Tileset::get_uid()
{
    return this->uid;
}

int Tileset::get_tileset_width()
{
    return this->tileset_width;
}

int Tileset::get_tileset_height()
{
    return this->tileset_height;
}

void Tileset::draw_tile(int tile_index, int x, int y , int w, int h)
{
    SDL_Rect dest = {x, y, w, h};
    SDL_RenderCopy(this->Camera->GetRenderer(), this->texture, this->tile_rects[get_tile_index(tile_index)], &dest);
}

int Tileset::get_tile_index(int tile_index)
{
    // TODO 
    return tile_index;
}