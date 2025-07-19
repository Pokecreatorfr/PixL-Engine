#include <PixL-Engine.hpp>
#include <SDL3/SDL_vulkan.h>
#include <chrono>

int main(int argc, char *argv[])
{
    PixL_Engine *engine = PixL_Engine::getInstance();
    printf("Video driver: %s\n", SDL_GetCurrentVideoDriver());
    while (!engine->IsQuitRequested())
    {
        engine->run();
    }

    return 0;
}