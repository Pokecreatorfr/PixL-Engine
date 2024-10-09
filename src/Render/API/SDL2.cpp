#include "SDL2.hpp"

void SDL2Renderer::init(SDL_Window *w)
{
    SDL_VideoInit(NULL);
    this->window = w;
    this->renderer = SDL_CreateRenderer(this->window, -1, SDL_RENDERER_ACCELERATED);
}

void SDL2Renderer::update()
{
}

void SDL2Renderer::render()
{
    SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 255);
    SDL_RenderClear(this->renderer);
    SDL_RenderPresent(this->renderer);
}

void SDL2Renderer::destroy()
{
    // close sdlvideo
    SDL_VideoQuit();
}
