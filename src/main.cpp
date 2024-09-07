#include <SDL2/SDL.h>
#include <iostream>
#include <logger.hpp>
#include <resources_manager.hpp>


int main( int argc, char* args[] )
{
    if( SDL_Init( SDL_INIT_VIDEO ) == -1 )
    {
        Logger::GetInstance()->log("SDL could not initialize! SDL_Error: " + std::string(SDL_GetError()));
        return -1;
    }

    SDL_Window* screen = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN );

    if( screen == NULL )
    {
        Logger::GetInstance()->log("Window could not be created! SDL_Error: " + std::string(SDL_GetError()));
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(screen, -1, SDL_RENDERER_ACCELERATED);

    if( renderer == NULL )
    {
        Logger::GetInstance()->log("Renderer could not be created! SDL_Error: " + std::string(SDL_GetError()));
        return -1;
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    bool quit = false;
    ResourcesManager* resourcesManager = ResourcesManager::getInstance();

    SDL_Event e;

    while( !quit )
    {
        while( SDL_PollEvent( &e ) != 0 )
        {
            if( e.type == SDL_QUIT )
            {
                quit = true;
            }
        }

        SDL_RenderClear(renderer);

        SDL_RenderPresent(renderer);
        resourcesManager->closeUnusedResources();
    }



    SDL_Quit();

    return 0;
}