#include <game/game.hpp>

using namespace game;

pixl::gfx::Window *Game::window = nullptr;
pixl::gfx::Gpu *Game::gpu = nullptr;
std::unique_ptr<World> Game::world = nullptr;

int Game::Init()
{
    pixl::core::Core::Init(pixl::core::CoreInitData{});
    pixl::gfx::GfxInitData gfxInit{};
    gfxInit.windowFlags = SDL_WINDOW_RESIZABLE;
    if (pixl::gfx::Gfx::Init(gfxInit) != 0)
    {
        return -1;
    }
    window = pixl::gfx::Gfx::GetWindow();
    gpu = pixl::gfx::Gfx::GetGpu();
    return 0;
}

int Game::Quit()
{
    if (!window || !gpu)
    {
        return -1;
    }

    pixl::gfx::Gfx::Quit();
    pixl::core::Core::Quit();
    return 0;
}

int Game::Run()
{
    if (!window || !gpu)
    {
        return -1;
    }

    return 0;
}