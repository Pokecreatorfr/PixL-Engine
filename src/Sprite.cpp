#include "Sprite.hpp"

SpriteRenderer::SpriteRenderer(SDL_Texture* texture, int height, int width)
{
    this->camera = Camera::GetInstance();
    this->texture = texture;
    this->height = height;
    this->width = width;
    // get texture size
    int texture_width;
    int texture_height;
    SDL_QueryTexture(texture, NULL, NULL, &texture_width, &texture_height);
    int number_of_frames = (texture_width / width) * (texture_height / height);
    for (int i = 0; i < number_of_frames; i++)
    {
        SDL_Rect frame;
        frame.x = (i * width) % texture_width;
        frame.y = (i * width) / texture_width * height;
        frame.w = width;
        frame.h = height;
        this->frames.push_back(frame);
    }
}

void SpriteRenderer::Draw_World_coord(coord_2d position, int index)
{

    if (check_rect_visibility(position, {this->width, this->height}, this->camera))
    {
        SDL_Rect dest;
        dest.x = (int)((position.x - this->camera->GetPosition()->x) * *this->camera->GetZoom() + this->camera->GetSize()->x / 2);
        dest.y = (int)((position.y - this->camera->GetPosition()->y) * *this->camera->GetZoom() + this->camera->GetSize()->y / 2);
        dest.w = this->width * *this->camera->GetZoom();
        dest.h = this->height * *this->camera->GetZoom();
        SDL_RenderCopy(this->camera->GetRenderer(), this->texture, &this->frames[index], &dest);
        #ifdef DEBUG
        SDL_SetRenderDrawColor(this->camera->GetRenderer(), 255, 0, 0, 255);
        SDL_RenderDrawRect(this->camera->GetRenderer(), &dest);
        #endif
    }
}

void SpriteRenderer::Draw_Screen_coord(coord_2d position, int index)
{
    SDL_Rect dest;
    dest.x = position.x;
    dest.y = position.y;
    dest.w = this->width;
    dest.h = this->height;
    SDL_RenderCopy(this->camera->GetRenderer(), this->texture, &this->frames[index], &dest);
}