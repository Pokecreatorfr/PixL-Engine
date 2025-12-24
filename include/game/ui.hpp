#pragma once
#include <asset_loader/asset_loader.hpp>
#include <retro_renderer/retro_renderer.hpp>

class UI
{
    UI();
    static UI *_instance;

public:
    static int Init();
    static int Render();
    static int Quit();

private:
};