#pragma once
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <const/Config.hpp>

using namespace std;

struct image_ressource
{
	const char* image_ressource;
	const int* image_size;
};

struct colorRGB
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
};

SDL_Texture* Load_Texture(image_ressource image_ressource, SDL_Renderer* renderer);