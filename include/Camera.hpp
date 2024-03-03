#pragma once
#include <const/Config.hpp>
#include <SDL2/SDL.h>
#include <Log.hpp>

struct coord_2d
{
    int x;
    int y;
};

class Camera
{
    protected:
        Camera();
        static Camera* instance_;
    public:
        static Camera* GetInstance();
        SDL_Window* GetWindow();
        SDL_Renderer* GetRenderer();
        coord_2d* GetPosition();
        coord_2d* GetSize();
        int GetFrame();
        float* GetZoom();
        void SetPosition(coord_2d position);
        void SetSize(coord_2d size);
        void SetZoom(float zoom);
        void addFrame();
    private:
        SDL_Window* window;
        SDL_Renderer* renderer;
        coord_2d position;
        coord_2d size;
        float zoom;
        int frame;
};