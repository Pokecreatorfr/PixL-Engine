#include <Renderer.hpp>


Renderer::Renderer(SDL_Window* window, overworld_vars* overworld_struct)
{
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        std::cout << "Failed to create renderer\n";
        return;
    }


    int w, h;
    SDL_GetWindowSize(window, &w, &h);

    for (int i = 0; i < 5; i++)
    {
        SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
        if (!texture)
        {
            std::cout << "Failed to create texture\n";
            return;
        }
        layers.push_back(texture);
    }
    this->overworld_struct = overworld_struct;
}

Renderer::~Renderer()
{
    for (int i = 0; i < layers.size(); i++)
    {
        SDL_DestroyTexture(layers[i]);
    }
    SDL_DestroyRenderer(renderer);
}

Overworld* Renderer::GetOverworld()
{
    return new Overworld(renderer, overworld_struct, layers);
}