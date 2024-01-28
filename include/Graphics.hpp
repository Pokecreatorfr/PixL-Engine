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

SDL_Texture* Load_Texture(image_ressource image_ressource, SDL_Renderer* renderer);