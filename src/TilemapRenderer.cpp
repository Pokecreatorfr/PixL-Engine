#include <TilemapRenderer.hpp>


TilemapRenderer::TilemapRenderer(camera* cam ,const tiles_layer* tl ,int x, int y , int w , int h ,  Tileset* ts)
{
    this->Camera = cam;
    this->height = h;
    this->width = w;
    this->tileset = ts;
    for(int i = 0 ; i < h ; i++)
    {
        for(int j = 0 ; j < w ; j++)
        {
            int tile_index = tl->tiles[i * w + j];
            if(tile_index != -1)
            {
                coord_2d position = {(j + x) * TILE_SIZE , (i + y) * TILE_SIZE};
                coord_2d size = {TILE_SIZE , TILE_SIZE};
                Tile tile = {position , size , tile_index};
                this->tiles.push_back(tile);
            }
        }
    }
}

TilemapRenderer::~TilemapRenderer()
{
}

int TilemapRenderer::get_height()
{
    return this->height;
}

int TilemapRenderer::get_width()
{
    return this->width;
}

void TilemapRenderer::draw()
{
    for(int i = 0 ; i < this->tiles.size() ; i++)
    {
        SDL_Rect rect;

        rect.x = (this->tiles[i].position.x + this->Camera->position.x) * this->Camera->zoom + Camera->size.x / 2;
        rect.y = (this->tiles[i].position.y + this->Camera->position.y) * this->Camera->zoom + Camera->size.y / 2;
        rect.w = this->tiles[i].size.x * this->Camera->zoom;
        rect.h = this->tiles[i].size.y * this->Camera->zoom;
        tileset->draw_tile(this->tiles[i].tile_index , rect.x , rect.y , rect.w , rect.h);
    }
}