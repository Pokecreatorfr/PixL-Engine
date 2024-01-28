#include <Light.hpp>

SDL_Texture* Generate_Light_Texture(SDL_Renderer* renderer, int radius, int r, int g, int b, int a)
{
    // generate transparent surface
    SDL_Surface* surface = SDL_CreateRGBSurface(0, radius * 2, radius * 2, 32, 0xFF000000,  0x00FF0000,  0x0000FF00,  0x000000FF);
    // set color mode to RGBA8888
    SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_BLEND);

    // draw on surface point by point
    for (int i = 0; i < radius * 2; i++)
    {
        for (int j = 0; j < radius * 2; j++)
        {
            // calculate distance from center
            int dx = radius - i;
            int dy = radius - j;
            float dist = sqrt(dx * dx + dy * dy);
            
            if(dist < radius)
            {
                // set pixel
                Uint32* pixel = (Uint32*)surface->pixels + i + j * surface->w;
                *pixel = SDL_MapRGBA(surface->format, r, g, b, a);
            }
            else
            {
                // set pixel
                Uint32* pixel = (Uint32*)surface->pixels + i + j * surface->w;
                *pixel = SDL_MapRGBA(surface->format, 0, 0, 0, 0);
            }
        }
    }

    // create texture from surface
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    
    // free surface
    SDL_FreeSurface(surface);

    return texture;
}