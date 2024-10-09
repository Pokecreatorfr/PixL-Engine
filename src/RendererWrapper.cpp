#include <Render/RendererWrapper.hpp>

RendererWrapper::RendererWrapper(RendererType type)
{
}

RendererWrapper::~RendererWrapper()
{
}

bool RendererWrapper::init()
{
    SDL_Init(SDL_INIT_VIDEO);
    return true;
}
SDL_Window *RendererWrapper::create_window(std::string title, int width, int height)
{
    SDL_Init(SDL_INIT_VIDEO);

    this->window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);

    return this->window;
}

bool RendererWrapper::destroy_window()
{
    SDL_DestroyWindow(this->window);
    return true;
}
