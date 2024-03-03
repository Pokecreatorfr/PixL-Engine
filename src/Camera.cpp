#include <Camera.hpp>


Camera::Camera()
{
    this->window = SDL_CreateWindow("PixL Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);;
    this->renderer = SDL_CreateRenderer(this->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);;
    this->position.x = 0;
    this->position.y = 0;
    this->size.x = WINDOW_WIDTH;
    this->size.y = WINDOW_HEIGHT;
    this->zoom = 1.0;
    this->frame = 0;
}

Camera* Camera::instance_ = nullptr;

Camera* Camera::GetInstance()
{
    if (instance_ == nullptr)
    {
        instance_ = new Camera();
    }
    return instance_;
}

SDL_Window* Camera::GetWindow()
{
    return this->window;
}

SDL_Renderer* Camera::GetRenderer()
{
    return this->renderer;
}

coord_2d* Camera::GetPosition()
{
    return &this->position;
}

coord_2d* Camera::GetSize()
{
    return &this->size;
}

int Camera::GetFrame()
{
    return this->frame;
}

float* Camera::GetZoom()
{
    return &this->zoom;
}

void Camera::SetPosition(coord_2d position)
{
    this->position = position;
}

void Camera::SetSize(coord_2d size)
{
    this->size = size;
}

void Camera::SetZoom(float zoom)
{
    this->zoom = zoom;
}

void Camera::addFrame()
{
    this->frame++;
}