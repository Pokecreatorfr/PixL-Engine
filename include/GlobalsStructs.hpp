#pragma once
#include <SDL2/SDL.h>

struct coord_2d
{
    int x;
    int y;
};

struct camera
{
    // create a camera costructor
    camera(SDL_Renderer* renderer, coord_2d position, coord_2d size, float zoom)
    {
        this->renderer = renderer;
        this->position = position;
        this->size = size;
        this->zoom = zoom;
    }
    SDL_Renderer* renderer;
    coord_2d position;
    coord_2d size;
    float zoom;
};