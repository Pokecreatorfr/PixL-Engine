#include "Sprite.hpp"

Sprite::Sprite(SDL_Texture* texture, int height, int width)
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

void Sprite::Draw_World_coord(coord_2d position, int index)
{
    coord_2d screen_position = {position.x - this->height * *this->camera->GetZoom() , position.y - this->width * *this->camera->GetZoom() / 2};
    SDL_Rect dest;
    dest.x = screen_position.x;
    dest.y = screen_position.y;
    dest.w = this->width * *this->camera->GetZoom();
    dest.h = this->height * *this->camera->GetZoom();
    SDL_RenderCopy(this->camera->GetRenderer(), this->texture, &this->frames[index], &dest);
}

void Sprite::Draw_Screen_coord(coord_2d position, int index)
{
    SDL_Rect dest;
    dest.x = position.x;
    dest.y = position.y;
    dest.w = this->width;
    dest.h = this->height;
    SDL_RenderCopy(this->camera->GetRenderer(), this->texture, &this->frames[index], &dest);
}