#include <iostream>
#include <logger.hpp>
#include <master.hpp>
#include <resources_manager.hpp>
#include <ui/ui.hpp>


int main( int argc, char* args[] )
{
    Master* master = Master::getInstance(SDL_INIT_VIDEO, "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN, -1, SDL_RENDERER_ACCELERATED);
    SDL_Renderer* renderer = master->getRenderer();

    ui::UI ui;
    ui::UIContainer container(0, 0, 800, 100, 0x0000FFFF);
    container.setVisibility(true);

    container.addButton({10, 10, 100, 100, 0x00FF00FF, 0xFF0000FF, 0x00FF00FF, nullptr, nullptr, nullptr, "Hello"});

    ui.addContainer(container);

    ResourcesManager* resourcesManager = ResourcesManager::getInstance();

    while( master->updateEvent() )
    {
        

        SDL_RenderClear(renderer);

        ui.handleEvents();
        SDL_RenderPresent(renderer);
        resourcesManager->closeUnusedResources();

        SDL_Delay(1000 / 60);
        
    }

    
    SDL_Quit();

    return 0;
}