#pragma once
#include <const/Config.hpp>
#include <Graphics.hpp>
#include <overworld_vars.hpp>

struct overworld_vars;

struct tile_animation
{
    int tile_index;
    std::vector<int> frames;
    int modulo;
};


struct tileset
{
    int uid;
    int tileset_width;
    int tileset_height;
    const image_ressource* ressource;
    std::vector<tile_animation> animations;
};


class Tileset
{
    public:
        Tileset(SDL_Renderer* renderer,const tileset* tileset, overworld_vars* overworld_struct);
        ~Tileset();
        void get_tile(int tile_index, SDL_Rect* tile);
        SDL_Texture* get_texture();
        int get_tileset_uid();
        void draw_tile(int tile_index, int x, int y, int h , int w );


    private:
        int uid;
        SDL_Texture* texture;
        std::vector<SDL_Rect> tiles;
        std::vector<tile_animation> animations;
        SDL_Renderer* renderer;
        overworld_vars* overworld_struct;
};