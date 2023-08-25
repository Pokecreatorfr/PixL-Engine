#pragma once
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <Config.hpp>

using namespace std;

struct image_ressource
{
	const char* image_ressource;
	const int* image_size;
};

struct coord_2d
{
	int x;
	int y;
};

struct Tile
{
	// constructor
	Tile(coord_2d position, coord_2d size, int tile_index) : position(position), size(size), tile_index(tile_index) {}
	coord_2d position;
	coord_2d size;
	int tile_index;
};

struct Tileset
{
	SDL_Texture* texture;
	int tile_width;
	int tile_height;
	int tileset_width;
	int tileset_height;
	vector<SDL_Rect> tiles;
};

Tileset* Load_Tileset(image_ressource image_ressource, SDL_Renderer* renderer , int tile_width, int tile_height);