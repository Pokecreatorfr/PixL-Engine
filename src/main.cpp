#include <iostream>
#include <logger.hpp>
#include <master.hpp>
#include <resources_manager.hpp>
#include <ui/uicontainer.hpp>


int main( int argc, char* args[] )
{
    Master* master = Master::getInstance(SDL_INIT_VIDEO, "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN, -1, SDL_RENDERER_ACCELERATED);
    SDL_Renderer* renderer = master->getRenderer();


    bool quit = false;
    ResourcesManager* resourcesManager = ResourcesManager::getInstance();

    while( !quit )
    {
        master->updateEvent();

        SDL_RenderClear(renderer);

        SDL_RenderPresent(renderer);
        resourcesManager->closeUnusedResources();
    }



    SDL_Quit();

    return 0;
}