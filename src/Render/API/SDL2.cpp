#include <Render/API/SDL2.hpp>

void SDL2Renderer::init(SDL_Window *w)
{
    // init sdl video
    SDL_Init(SDL_INIT_VIDEO);
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

SDL_Texture *SDL2Renderer::getTexture(std::string path)
{
    // check if texture already loaded
    for (auto &texture : this->textures)
    {
        if (texture.second == path)
        {
            return texture.first;
        }
    }

    // load texture
    SDL_Surface *surface = SDL_LoadBMP(path.c_str());
    SDL_Texture *texture = SDL_CreateTextureFromSurface(this->renderer, surface);
    SDL_FreeSurface(surface);
    this->textures.push_back({texture, path});
    return texture;
}

void SDL2Renderer::removeTexture(std::string path)
{
    for (auto it = this->textures.begin(); it != this->textures.end(); ++it)
    {
        if (it->second == path)
        {
            this->textures.erase(it);
            return;
        }
    }
}

void SDL2Renderer::clearTextures()
{
    this->textures.clear();
}