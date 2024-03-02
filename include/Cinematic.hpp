#pragma once 
#include <Graphics.hpp>
#include <Camera.hpp>

typedef bool (*PtrCinematic)(Camera*, int* , std::vector<SDL_Texture*>*);

class Cinematic {
    public:
        Cinematic();
        ~Cinematic();
        void set_cinematic(PtrCinematic cinematic);
        void draw_cinematic();

    private:
    Camera* Camera;
    int current_frame;
    PtrCinematic cinematic = nullptr;
    std::vector<SDL_Texture*> textures;
};