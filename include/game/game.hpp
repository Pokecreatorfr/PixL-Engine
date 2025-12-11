#pragma once
#include <core/Core.hpp>
#include <game/world.hpp>
#include <gfx/gfx.hpp>

namespace game
{
    class Game
    {
        static pixl::gfx::Window *window;
        static pixl::gfx::Gpu *gpu;
        static std::unique_ptr<World> world;

    public:
        static int Init();
        static int Quit();

        static int Run();
    };
}
