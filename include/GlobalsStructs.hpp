#pragma once
#include <SDL2/SDL.h>
#include <Log.hpp>

struct coord_2d
{
    int x;
    int y;
};

struct camera
{
    SDL_Renderer* renderer;
    coord_2d position;
    coord_2d size;
    float zoom;
    Logger* logger;
};