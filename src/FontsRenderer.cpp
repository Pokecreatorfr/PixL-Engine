#include <FontsRenderer.hpp>

FontsRenderer::FontsRenderer( const font_ressource* f)
{
    this->cam = Camera::GetInstance();
    this->logger = Logger::GetInstance();
    this->uid = f->uid;
    for (int i = 0; i < f->fonts.size(); i++)
    {
        font_textures.push_back(Load_Texture(*f->fonts[i]->font_image, cam->GetRenderer()));
    }
}

FontsRenderer::~FontsRenderer()
{
    for (int i = 0; i < font_textures.size(); i++)
    {
        SDL_DestroyTexture(font_textures[i]);
    }
}

void FontsRenderer::render_text(int x, int y, int h, int w, int font_size, char16_t text, colorRGB color)
{
    // check if font size is available
    bool font_available = false;

    int font_index;
    for (int i = 0; i < sizeof(fonts_sizes) / sizeof(int); i++)
    {
        if (fonts_sizes[i] == font_size)
        {
            font_available = true;
            font_index = i;
            break;
        }
    }
    if (!font_available)
    {
        this->logger->log("Font size not available");
        return;
    }

    int textureh;
    int texturew;
    SDL_QueryTexture(font_textures[font_index], NULL, NULL, &texturew, &textureh);
    textureh /= fonts_sizes[font_index];
    texturew /= fonts_sizes[font_index];
    int total_size = textureh * texturew;

    // convert char16_t* to int16_t
    int16_t char_index = (int16_t)text;
    if (char_index > total_size)
    {
        this->logger->log("Character not available in font");
        return;
    }
    int char_x = char_index % texturew;
    int char_y = char_index / texturew;

    // render text
    SDL_Rect src = {char_x * fonts_sizes[font_index], char_y * fonts_sizes[font_index], fonts_sizes[font_index], fonts_sizes[font_index]};
    SDL_Rect dst = {x, y, h, w};
    SDL_SetTextureColorMod(font_textures[font_index], color.r, color.g, color.b);
    SDL_RenderCopy(cam->GetRenderer(), font_textures[font_index], &src, &dst);
    return;
}

void FontsRenderer::render_text(int x, int y, int h, int w, int font_size, const char16_t* text, colorRGB color)
{
    char16_t tmp = (char16_t)*text;
    this->render_text(x, y, h, w, font_size, tmp, color);
}