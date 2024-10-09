#pragma once
#include <Render/API/ApiModel.hpp>
#include <SDL2/SDL.h>

using namespace std;

class SDL2Renderer : public ApiModel
{
public:
    void init(SDL_Window *w) override;
    void update() override;
    void render() override;
    void destroy() override;
    void setWindow(SDL_Window *w) { this->window = w; }

private:
    SDL_Window *window;
    SDL_Renderer *renderer;
};