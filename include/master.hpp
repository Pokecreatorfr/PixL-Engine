#pragma once 
#include <SDL2/SDL.h>
#include <logger.hpp>


class Master {
protected:
    Master(Uint32 initFlags ,const char* title, int x, int y, int w, int h, Uint32 windowFlags , int rendererIndex, Uint32 rendererFlags);
    ~Master();
    static Master* instance_;
public:
    static Master* getInstance(Uint32 initFlags ,const char* title, int x, int y, int w, int h, Uint32 windowFlags , int rendererIndex, Uint32 rendererFlags);
    static Master* getInstance();
    SDL_Window* getWindow();
    SDL_Renderer* getRenderer();
    bool updateEvent();
    SDL_Event* getEvent();
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Event e;
};