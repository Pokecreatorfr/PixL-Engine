#pragma once

#include <PixL-Input-Handler.hpp>
#include <PixL_2D.hpp>
#include <PixL_Renderer.hpp>
#include <modules/PixL-Sprite.hpp>
#include <modules/PixL-Tilemap.hpp>

class PixL_Input_Handler;

class PixL_Engine
{
protected:
    PixL_Engine();
    ~PixL_Engine();
    static PixL_Engine *_instance;

public:
    static PixL_Engine *getInstance()
    {
        if (!_instance)
        {
            _instance = new PixL_Engine();
        }
        return _instance;
    }
    void quit();

    void run();

    bool IsQuitRequested()
    {
        return quitRequested;
    }

private:
    SDL_Window *window;
    bool running;
    bool initialized;
    PixL_Input_Handler *inputHandler;
    bool quitRequested = false;
    PixL_Tilemap *tilemap;
    PixL_Sprite *sprite;

    float rot = 0.0f; // Rotation angle for sprite rendering

    // Add any other member variables or functions as needed
};