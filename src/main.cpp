#include <PixL-Engine.hpp>
#include <chrono>

int main(int argc, char *argv[])
{

    PixL_Engine *engine = PixL_Engine::getInstance();

    while (!engine->IsQuitRequested())
    {
        engine->run();
    }

    return 0;
}