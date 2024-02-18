#pragma once
#include <Fonts.hpp>
#include <GlobalsStructs.hpp>
#include <const/Config.hpp>



class FontsRenderer
{
    public:
        FontsRenderer(camera* cam, const font_ressource* font);
        ~FontsRenderer();
        void render_text(int x, int y, int h, int w, int font_size, char16_t text, colorRGB color);
        void render_text(int x, int y, int h, int w, int font_size, const char16_t* text, colorRGB color);
    private:
        int uid;
        camera* cam;
        std::vector<SDL_Texture*> font_textures;
};