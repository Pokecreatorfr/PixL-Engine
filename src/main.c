#include <stddef.h>


#include "virtuappu.h"
#include "engine/command_parser.h"

#include "SDL3/SDL.h"
#include <stdio.h>

bool isSDLInitialized = false;

int main(int argc, char *argv[])
{
    PPUMemory *ppu = NULL;
    command_parser_parse(argc, argv);

    if (help_used) {
        command_parser_print_help((argc > 0) ? argv[0] : 0);
        return 0;
    }

    if (!no_display_used) {
        if (SDL_Init(SDL_INIT_VIDEO) != true) {
            return 1;
        }
        isSDLInitialized = true;

        SDL_Window *window = SDL_CreateWindow("PixL-Engine", 640, 480, 0);
        if (window == NULL) {
            if (isSDLInitialized) {
                SDL_Quit();
            }
            return 1;
        }
    }

    if (!no_input_used) {
        if (SDL_Init(SDL_INIT_EVENTS) != true) {
            if (isSDLInitialized) {
                SDL_Quit();
            }
            return 1;
        }
        isSDLInitialized = true;
    }

    if (!no_audio_used) {
        if (SDL_Init(SDL_INIT_AUDIO) != true) {
            if (isSDLInitialized) {
                SDL_Quit();
            }
            return 1;
        }
        isSDLInitialized = true;
    }

    virtuappu_reset();
    ppu = virtuappu_get_registers();
    ppu->frame_width = 240;
    ppu->mode = 0;
    virtuappu_render_frame();

    bool running = true;
    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }


        virtuappu_render_frame();
        
    }
    
    
    if (isSDLInitialized) {
        SDL_Quit();
    }
    return 0;
}
