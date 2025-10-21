#include <PixL-Engine.hpp>
#include <SDL3/SDL_vulkan.h>
#include <chrono>
#include <modules/PixL-Imgui.hpp>

int main(int argc, char *argv[])
{
    PixL_Engine *engine = PixL_Engine::getInstance();
    printf("Video driver: %s\n", SDL_GetCurrentVideoDriver());

    while (!engine->IsQuitRequested())
    {
        engine->run();
    }
    PixL_Engine::destroyInstance();
    return 0;
}
