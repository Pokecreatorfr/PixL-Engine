#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <vector>
#include <const/Config.hpp>
#include <overworld.hpp>

class Renderer 
{
    public:
        Renderer(SDL_Window* window, overworld_vars* overworld_struct, Logger* logger);
        ~Renderer();
        Overworld* GetOverworld();
        void draw();

    private:
    std::vector<SDL_Texture*> layers;
    SDL_Renderer* renderer;
    overworld_vars* overworld_struct;
    Logger* logger;
};