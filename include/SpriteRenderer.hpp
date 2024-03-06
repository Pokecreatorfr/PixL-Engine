#pragma once
#include <Graphics.hpp>
#include <Camera.hpp>
#include <maths.hpp>


struct sprite
{
    SDL_Texture* texture;
    int height;
    int width;
    coord_2d hitbox;
    // boucles d'animation
    std::vector<int> face_down;
    std::vector<int> face_up;
    std::vector<int> face_left;
    std::vector<int> face_right;
    std::vector<int> walk_down;
    std::vector<int> walk_up;
    std::vector<int> walk_left;
    std::vector<int> walk_right;
    std::vector<int> run_down;
    std::vector<int> run_up;
    std::vector<int> run_left;
    std::vector<int> run_right;
};

class SpriteRenderer
{
    public:
        SpriteRenderer(SDL_Texture* texture, int height, int width);
        void Draw_World_coord(coord_2d position , int index);
        void Draw_Screen_coord(coord_2d position , int index);
    private:
        Camera* camera;
        SDL_Texture* texture;
        int height;
        int width;
        std::vector<SDL_Rect> frames;
};