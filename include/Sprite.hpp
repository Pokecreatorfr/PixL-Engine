#pragma once
#include <Graphics.hpp>
#include <Camera.hpp>
#include <maths.hpp>




class Sprite
{
    public:
        Sprite(SDL_Texture* texture, int height, int width);
        void Draw_World_coord(coord_2d position , int index);
        void Draw_Screen_coord(coord_2d position , int index);
    private:
        Camera* camera;
        SDL_Texture* texture;
        int height;
        int width;
        std::vector<SDL_Rect> frames;
};