#pragma once 
#include <SpriteRenderer.hpp>


class OverworldCharacterRenderer
{
    public:
        OverworldCharacterRenderer(sprite* sprite, int height, int width);
        void Draw(coord_2d position , int index);
    private:
        SpriteRenderer* sprite_renderer;
        int height;
        int width;
        std::vector<SDL_Rect> frames;
        sprite* sprite;
};