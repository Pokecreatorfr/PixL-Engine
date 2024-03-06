#pragma once
#include <GuiRenderer.hpp>
#include <generated/image2cpp.hpp>


void lifebar(Camera* cam, int* w , int* h , int* x , int* y , std::vector<SDL_Texture*>* textures, FontsRenderer* font_renderer  , gui_param param)
{
    if (*param.bool_ptr_params.at(0))
    {
        std::string lifestring = std::to_string(*param.int_ptr_params.at(0)) + "/" + std::to_string(*param.int_ptr_params.at(1));
        char16_t* lifechar = new char16_t[lifestring.size() + 1];
        for (int i = 0; i < lifestring.size(); i++)
        {
            lifechar[i] = std::u16string(1, lifestring[i])[0];
        }

        if(textures->size() == 0)
        {
            SDL_Texture* render_texture = SDL_CreateTexture(cam->GetRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 104, 47);
            textures->push_back(render_texture);
            SDL_Texture* texture = Load_Texture(gui_lifebar_ressource, cam->GetRenderer());
            textures->push_back(texture);
        }
        SDL_SetRenderTarget(cam->GetRenderer(), textures->at(0));
        SDL_SetRenderDrawColor(cam->GetRenderer(), 0, 0, 0, 255);
        SDL_RenderClear(cam->GetRenderer());
        SDL_Rect dest = {0, 0, 104, 47};
        SDL_Rect src = {0, 5, 104, 47};
        SDL_RenderCopy(cam->GetRenderer(), textures->at(1), &src, &dest);
        int lifebar_size = ((float)*param.int_ptr_params.at(0) / (float)*param.int_ptr_params.at(1)) * 100;
        dest = {2, 2, lifebar_size, 5};
        src = {0, 0, lifebar_size, 5};
        SDL_RenderCopy(cam->GetRenderer(), textures->at(1), &src, &dest);
        for (int i = lifestring.size() ; i >= 0; i--)
        {
            font_renderer->render_text( 104 - (i + 1)* 11  , 20, 32, 32, 32,lifechar[lifestring.size() - i - 1],{0,0,0});
        }
        SDL_SetRenderTarget(cam->GetRenderer(), NULL);
        int text_h = 47 * *w / 104;
        SDL_Rect dest2 = { *x, *y, *w, text_h};
        SDL_RenderCopy(cam->GetRenderer(), textures->at(0), NULL, &dest2);
        delete[] lifechar;
    }
    return;
}

void caminfo(Camera* cam, int* w , int* h , int* x , int* y , std::vector<SDL_Texture*>* textures, FontsRenderer* font_renderer  , gui_param param)
{
    if (*param.bool_ptr_params.at(0))
    {
        std::string xstring = "X:" + std::to_string(cam->GetPosition()->x);
        std::string ystring = "Y:" + std::to_string(cam->GetPosition()->y);
        std::string zoomstring = "Zoom:" + std::to_string(*cam->GetZoom());
        char16_t* xchar = new char16_t[xstring.size() + 1];
        char16_t* ychar = new char16_t[ystring.size() + 1];
        char16_t* zoomchar = new char16_t[zoomstring.size() + 1];
        for (int i = 0; i < xstring.size(); i++)
        {
            xchar[i] = std::u16string(1, xstring[i])[0];
        }
        for (int i = 0; i < ystring.size(); i++)
        {
            ychar[i] = std::u16string(1, ystring[i])[0];
        }
        for (int i = 0; i < zoomstring.size(); i++)
        {
            zoomchar[i] = std::u16string(1, zoomstring[i])[0];
        }
        if(textures->size() == 0)
        {
            SDL_Texture* render_texture = SDL_CreateTexture(cam->GetRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 310, 34);
            textures->push_back(render_texture);
        }
        SDL_SetRenderTarget(cam->GetRenderer(), textures->at(0));
        SDL_SetRenderDrawColor(cam->GetRenderer(), 255, 255, 255, 255);
        SDL_RenderClear(cam->GetRenderer());
        for (int i = 0 ; i < xstring.size(); i++)
        {
            font_renderer->render_text( 2 + (i + 1)* 11  , 2, 32, 32, 32,xchar[i],{0,0,0});
        }
        for (int i = 0 ; i < ystring.size(); i++)
        {
            font_renderer->render_text( 79 + (i + 1)* 11  , 2, 32, 32, 32,ychar[i],{0,0,0});
        }
        for (int i = 0 ; i < zoomstring.size(); i++)
        {
            font_renderer->render_text( 156 + (i + 1)* 11  , 2, 32, 32, 32,zoomchar[i],{0,0,0});
        }
        int text_h = 15 * *w / 169;
        SDL_Rect dest = { *x, *y, *w, text_h};
        SDL_SetRenderTarget(cam->GetRenderer(), NULL);
        SDL_RenderCopy(cam->GetRenderer(), textures->at(0), NULL, &dest);
    }
    return;
}

void textbox(Camera* cam, int* w , int* h , int* x , int* y , std::vector<SDL_Texture*>* textures, FontsRenderer* font_renderer  , gui_param param)
{
    if(textures->size() == 0)
    {
        SDL_Texture* render_texture = SDL_CreateTexture(cam->GetRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, *w, *h);
        textures->push_back(render_texture);

    }
    return;
}