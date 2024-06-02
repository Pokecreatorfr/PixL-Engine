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
        int get_tile_width();
        int get_tile_height();
        int get_uid();
        OpenGLTexture* get_texture();
    private:
        OpenGLTexture* texture;
        int tile_width;
        int tile_height;
        std::vector<tile_animation> animations;
        int uid;
};