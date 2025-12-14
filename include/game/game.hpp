#pragma once
#include <asset_loader/asset_loader.hpp>
#include <retro_renderer/retro_renderer.hpp>

class Game
{
    Game();
    static Game *_instance;

public:
    static int Init(bool *running);
    static int Run();
    static int Quit();

private:
    bool *running;
};