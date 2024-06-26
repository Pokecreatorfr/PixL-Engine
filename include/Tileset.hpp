#pragma once
#include <const/Config.hpp>
#include <Graphics.hpp>
#include <Camera.hpp>

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
        Tileset(const tileset* tileset);
        ~Tileset();
        int get_uid();
        int get_tileset_width();
        int get_tileset_height();
        void draw_tile(int tile_index, int x, int y , int w, int h);
    private:
        int uid;
        int tileset_width;
        int tileset_height;
        SDL_Texture *texture;
        std::vector<SDL_Rect*> tile_rects;
        std::vector<tile_animation> animations;
        Camera* Camera;
        int get_tile_index(int tile_index);
};
