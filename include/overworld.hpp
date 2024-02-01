#pragma once
#include <tileset.hpp>
#include <map_struct.hpp>
#include <generated/map2cpp.hpp>
#include <const/const_renderer.hpp>
#include <Log.hpp>
#include <rendering_funct.hpp>


class Overworld
{
    public:
    Overworld(SDL_Renderer* renderer, overworld_vars* overworld_struct, std::vector<SDL_Texture*> layers , Logger* logger);
    ~Overworld();
    void load_tileset(const tileset* tileset);
    void load_map(const map_struct* map);
    void unload_map(int index);
    void update();
    void draw();

    private:
    SDL_Renderer* renderer;
    std::vector<Tileset*> tilesets;
    std::vector<const map_struct*> maps;
    overworld_vars* overworld_struct;
    std::vector<SDL_Texture*> layers;
    Logger* logger;
};

