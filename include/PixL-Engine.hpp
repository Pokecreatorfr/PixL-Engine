#pragma once

#include <Imgui_Layouts/Imgui_debug_menu.hpp>
#include <Particles/Fire.hpp>
#include <PixL-Input-Handler.hpp>
#include <PixL-Ressource.hpp>
#include <PixL_2D.hpp>
#include <PixL_Renderer.hpp>
#include <modules/PixL-Imgui.hpp>
#include <modules/PixL-Particle.hpp>
#include <modules/PixL-Sprite.hpp>
#include <modules/PixL-Tilemap.hpp>
#ifdef PIXL_STUDIO
#include <PixL-Studio.hpp>
#endif

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

    bool ToogleFullscreen();

private:
    SDL_Window *window;
    bool running;
    bool initialized;
    PixL_Input_Handler *inputHandler;
    bool quitRequested = false;
    PixL_Tilemap *tilemap;
    PixL_Sprite *sprite;
    PixL_Particle *particleSystem;

    float rot = 0.0f; // Rotation angle for sprite rendering

    // Add any other member variables or functions as needed
};