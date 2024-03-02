#pragma once 
#include <vector>
#include <SDL2/SDL.h>
#include <Camera.hpp>
#include <MapStruct.hpp>
#include <maths.hpp>


struct Tile
{
	// constructor
	Tile(coord_2d position, coord_2d size, int tile_index) : position(position), size(size), tile_index(tile_index) {}
	coord_2d position;
	coord_2d size;
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
    private:
        int height;
        int width;
        Camera* Camera;
        std::vector<Tile> tiles;
        Tileset* tileset;
};