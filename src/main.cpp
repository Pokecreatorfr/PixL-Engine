#include <SDL2/SDL.h>
#include <iostream>


int main( int argc, char* args[] )
{
    //Start SDL
    if( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
    {
        return 1;
    }

    //Quit SDL
    SDL_Quit();

    return 0;
}