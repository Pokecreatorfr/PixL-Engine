#pragma once
#include <Fonts.hpp>
#include <Camera.hpp>
#include <const/Config.hpp>



class FontsRenderer
{
    public:
        FontsRenderer(const font_ressource* font);
        ~FontsRenderer();
        void render_text(int x, int y, int h, int w, int font_size, char16_t text, colorRGB color);
        void render_text(int x, int y, int h, int w, int font_size, const char16_t* text, colorRGB color);
    private:
        int uid;
        Camera* cam;
        std::vector<SDL_Texture*> font_textures;
        Logger* logger;
};