#pragma once
#include <core/Core.hpp>
#include <core/maths/Maths.hpp>
#include <game/entities.hpp>
#include <game/types.hpp>
#include <gfx/gfx.hpp>
#include <nlohmann/json.hpp>

namespace game
{
    struct WorldDescriptor
    {
        std::string name;
        std::vector<MapDescriptor> maps;
        std::vector<Entity> entities;
    };

    class World
    {
        static std::vector<MapDescriptor> maps;
        static std::vector<Entity> entities;
        static bool isInitialized;

    public:
        static int Init(WorldDescriptor &descriptor);
        static int Quit();

        static int RunFrame(float deltaTime);
    };

}