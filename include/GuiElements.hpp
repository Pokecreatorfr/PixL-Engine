#pragma once
#include <GuiRenderer.hpp>
#include <generated/image2cpp.hpp>

void lifebar(camera* cam, int* w , int* h , int* x , int* y , std::vector<SDL_Texture*>* textures, FontsRenderer* font_renderer  , gui_param param)
{
    //cam->logger->log("x:" + std::to_string(*x) + " y:" + std::to_string(*y) + " w:" + std::to_string(*w) + " lifebar" );
    if(textures->size() == 0)
    {
        SDL_Texture* texture = Load_Texture(gui_lifebar_ressource, cam->renderer);
        textures->push_back(texture);
    }
    SDL_Texture* render_texture = SDL_CreateTexture(cam->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 104, 9);
    SDL_SetRenderTarget(cam->renderer, render_texture);
    SDL_SetRenderDrawColor(cam->renderer, 0, 0, 0, 255);
    SDL_RenderClear(cam->renderer);
    SDL_Rect dest = {0, 0, 104, 9};
    SDL_Rect src = {0, 5, 104, 9};
    SDL_RenderCopy(cam->renderer, textures->at(0), &src, &dest);
    int lifebar_size = ((float)*param.int_param1 / (float)*param.int_param2) * 100;
    dest = {2, 2, lifebar_size, 5};
    src = {0, 0, lifebar_size, 5};
    SDL_RenderCopy(cam->renderer, textures->at(0), &src, &dest);
    SDL_SetRenderTarget(cam->renderer, NULL);
    int text_h = 9 * *w / 104;
    SDL_Rect dest2 = { *x, *y, *w, text_h};
    SDL_RenderCopy(cam->renderer, render_texture, NULL, &dest2);
    SDL_DestroyTexture(render_texture);
    return;
}