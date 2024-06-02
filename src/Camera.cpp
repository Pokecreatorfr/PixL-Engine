#include "Camera.hpp"

Camera::Camera()
{
    this->posx = 0;
    this->posy = 0;
    this->zoom = 1;
    this->window = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (window == nullptr)
    {
        std::cout << "Failed to create window\n";
        return;
    }
    this->context = SDL_GL_CreateContext(window);
    this->size.x = 800;
    this->size.y = 600;
    this->renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    this->square_size = screen_square_size(size.x, size.y);
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

void Camera::Update()
{
    SDL_GetWindowSize(window, &size.x, &size.y);
}

void Camera::SetPosition(float x, float y)
{
    this->posx = x;
    this->posy = y;
}

void Camera::SetZoom(float zoom)
{
    this->zoom = zoom;
}

void Camera::Set_Square_Size(SDL_FPoint square_size)
{
    this->square_size = square_size;
}

float* Camera::Getposx()
{
    return &this->posx;
}

float* Camera::Getposy()
{
    return &this->posy;
}

float* Camera::GetZoom()
{
    return &this->zoom;
}

SDL_Point *Camera::GetSize()
{
    return &size;
}

SDL_Window* Camera::GetWindow()
{
    return this->window;
}

SDL_FPoint *Camera::Get_Square_Size()
{
    return &this->square_size;
}
