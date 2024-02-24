#pragma once 
#include <Graphics.hpp>
#include <GlobalsStructs.hpp>

typedef bool (*PtrCinematic)(camera*, int* , std::vector<SDL_Texture*>*);

class Cinematic {
    public:
        Cinematic(camera* cam);
        ~Cinematic();
        void set_cinematic(PtrCinematic cinematic);
        void draw_cinematic();

    private:
    camera* Camera;
    int current_frame;
    PtrCinematic cinematic = nullptr;
    std::vector<SDL_Texture*> textures;
};