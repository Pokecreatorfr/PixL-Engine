#pragma once
#include <SDL2/SDL.h>

class ApiModel
{
public:
    virtual void init(SDL_Window *) = 0;
    virtual void update() = 0;
    virtual void render() = 0;
    virtual void destroy() = 0;
};