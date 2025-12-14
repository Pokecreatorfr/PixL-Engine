#include <game/game.hpp>

int main(int argc, char *argv[])
{

    bool running = true;
    if (Game::Init(&running) != 0)
    {
        return -1;
    }
    while (running)
    {
        if (Game::Run() != 0)
        {
            break;
        }
    }

    if (Game::Quit() != 0)
    {
        return -1;
    }

    return 0;
}
