#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <master.hpp>

SDL_Texture *loadTexture(const char *filename, SDL_Renderer *renderer);