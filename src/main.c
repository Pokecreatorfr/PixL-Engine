#include <stddef.h>


#include "virtuappu.h"
#include "engine/command_parser.h"

#include "SDL3/SDL.h"

bool isSDLInitialized = false;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *texture = NULL;

int main(int argc, char *argv[])
{
    PPUMemory *ppu = NULL;
    command_parser_parse(argc, argv);

    virtuappu_reset();
    ppu = virtuappu_get_registers();
    ppu->frame_width = 240;
    ppu->mode = 0;

    if (help_used) {
        command_parser_print_help((argc > 0) ? argv[0] : 0);
        return 0;
    }

    if (!no_display_used) {
        if (SDL_Init(SDL_INIT_VIDEO) != true) {
            return 1;
        }
        isSDLInitialized = true;

        window = SDL_CreateWindow("PixL-Engine", 480, 360, 0);
        if (window == NULL) {
            if (isSDLInitialized) {
                SDL_Quit();
            }
            return 1;
        }

        renderer = SDL_CreateRenderer(window, NULL);
        if (renderer == NULL) {
            if (isSDLInitialized) {
                SDL_Quit();
            }
            return 1;
        }

        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, ppu->frame_width, 320);
        if (texture == NULL) {
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
        if (!no_display_used) {
            SDL_UpdateTexture(texture, NULL, virtuappu_get_frame_buffer(), ppu->frame_width * sizeof(uint32_t));
            SDL_RenderClear(renderer);
            SDL_RenderTexture(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);
        }
    }
    
    
    if (isSDLInitialized) {
        SDL_Quit();
    }
    return 0;
}