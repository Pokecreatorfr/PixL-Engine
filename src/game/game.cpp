#include <game/game.hpp>

Game *Game::_instance = nullptr;

Game::Game()
    : running(nullptr) {}

int Game::Init(bool *running)
{
    if (RetroRenderer::Init(800, 600, "Game Window") != 0)
    {
        return -1;
    }
    _instance = new Game();
    _instance->running = running;
    *running = true;

    return 0;
}

int Game::Run()
{
    if (_instance == nullptr)
        return -1; // Not initialized

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_EVENT_QUIT)
        {
            *_instance->running = false;
        }
    }

    RetroRenderer::BeginFrame();
    RetroRenderer::RenderFrame();

    return 0;
}

int Game::Quit()
{
    if (_instance == nullptr)
        return -1; // Not initialized
    RetroRenderer::Quit();
    delete _instance;
    return 0;
}