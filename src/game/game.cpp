#include <game/game.hpp>
#include <iostream>
#include <main_loop.hpp>

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
    InputHandler::Init();
    InputHandler::Bind_QuitRequest(&_instance->quit_requested);
    MainLoop_Init(running);

    return 0;
}

int Game::Run()
{
    if (_instance == nullptr)
        return -1;
    InputHandler::Update();
    if (_instance->quit_requested)
    {
        *(_instance->running) = false;
    }

    MainLoop_Run();

    return 0;
}

int Game::Quit()
{
    if (_instance == nullptr)
        return -1;
    RetroRenderer::Quit();
    MainLoop_Quit();
    delete _instance;
    return 0;
}