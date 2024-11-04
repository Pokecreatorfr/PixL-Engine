#pragma once
#include <Render/API/ApiModel.hpp>
#include <SDL2/SDL.h>
#include <string>
#include <utility>
#include <vector>

using namespace std;

class SDL2Renderer : public ApiModel
{
public:
    void init(SDL_Window *w) override;
    void update() override;
    void render() override;
    void destroy() override;
    void setWindow(SDL_Window *w) { this->window = w; }

    // SDL2 specific functions
    SDL_Texture *getTexture(std::string path);
    void removeTexture(std::string path);
    void clearTextures();
    SDL_Window *getWindow() { return this->window; }
    SDL_Renderer *getRenderer() { return this->renderer; }

private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    std::vector<std::pair<SDL_Texture *, std::string>> textures;
};