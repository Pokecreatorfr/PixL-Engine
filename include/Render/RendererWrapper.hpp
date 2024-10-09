#pragma once
#include <Render/API/ApiModel.hpp>
#include <Render/API/SDL2.hpp>
#include <SDL2/SDL.h>
#include <string>

enum RendererType
{
    SDL2,
    Vulkan,
    OpenGL,
    DirectX
};

class RendererWrapper
{
public:
    RendererWrapper(RendererType type);
    ~RendererWrapper();
    void init();
    SDL_Window *create_window(std::string title, int width, int height);
    bool destroy_window();

private:
    RendererType renderer_type;
    SDL_Window *window;
    ApiModel *renderer;
};