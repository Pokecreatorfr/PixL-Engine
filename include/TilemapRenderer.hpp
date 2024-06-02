#pragma once 

#include <Tileset.hpp>
#include <Renderer.hpp>
#include <generated/map2cpp.hpp>

struct Tile
{
	// constructor
	Tile(SDL_Point position, SDL_Point size, int tile_index) : position(position), size(size), tile_index(tile_index) {}
	SDL_Point position;
	SDL_Point size;
	int tile_index;
};

class TilemapRenderer
{
    public:
        TilemapRenderer(const tiles_layer* tl ,int x , int y , int w , int h ,  Tileset* ts);
        ~TilemapRenderer();
        int get_height();
        int get_width();
        void draw();
        Tileset* get_tileset();
    private:
        int height;
        int width;
        Camera* Camera;
        std::vector<Tile> tiles;
        Tileset* tileset;
        unsigned int VAO , VBO , EBO;
        float* vertices;
        unsigned int* indices;

};
