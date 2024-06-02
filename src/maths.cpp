#include <maths.hpp>

SDL_FPoint screen_square_size(int w, int h)
{
    if (w > h)
    {
        return {1.0f, (float)h / (float)w};
    }
    else
    {
        return {(float)w / (float)h, 1.0f};
    }
}

bool check_overworld_rect_visibility(float x, float y, float w, float h)
{
    Camera* camera = Camera::GetInstance();
    float zoom = *camera->GetZoom();
    float posx = *camera->Getposx();
    float posy = *camera->Getposy();
    SDL_FPoint screen_square_size = *camera->Get_Square_Size();

    if(x * screen_square_size.x < posx + 1.0f * zoom && (x + w) * screen_square_size.x > posx - 1.0f * zoom && y * screen_square_size.y < posy + 1.0f * zoom && (y + h) * screen_square_size.y > posy - 1.0f * zoom)
    {
        return true;
    }
    return false;
}