#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <maths.hpp>


class Camera
{
    protected:
        Camera();
        static Camera* instance_;
    public:
        static Camera* GetInstance();
        void Update();
        void SetPosition(float x, float y);
        void SetZoom(float zoom);
        void Set_Square_Size(SDL_FPoint square_size);
        float* Getposx();
        float* Getposy();
        float* GetZoom();
        SDL_Point* GetSize();
        SDL_Window* GetWindow();
        SDL_FPoint* Get_Square_Size();
    private:
        float posx;
        float posy;
        float zoom;
        SDL_Window* window;
        SDL_Point size;
        SDL_Renderer* renderer;
        SDL_GLContext context;
        SDL_FPoint square_size;
        

};