#include <core/Errors.hpp>
#include <game/world.hpp>

using namespace game;

std::vector<MapDescriptor> World::maps;
std::vector<Entity> World::entities;
bool World::isInitialized = false;

int World::Init(WorldDescriptor &descriptor)
{
    if (isInitialized)
        PIXL_LOG_AND_RETURN_ERROR(-1, pixl::core::Errc::AlreadyInitialisedSubsystem, "World", "World already initialized");
    maps = descriptor.maps;
    entities = descriptor.entities;
    isInitialized = true;
    return 0;
}

int World::Quit()
{
    if (!isInitialized)
        PIXL_LOG_AND_RETURN_ERROR(-1, pixl::core::Errc::UninitialisedSubsystem, "World", "World not initialized");

    return 0;
}

int World::RunFrame(float deltaTime)
{
    if (!isInitialized)
        PIXL_LOG_AND_RETURN_ERROR(-1, pixl::core::Errc::UninitialisedSubsystem, "World", "World not initialized");

    return 0;
}
