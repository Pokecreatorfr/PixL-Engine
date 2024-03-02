#include <Cinematic.hpp>

Cinematic::Cinematic(){
    Camera = Camera::GetInstance();
    current_frame = 0;
}

Cinematic::~Cinematic(){
    for (auto texture : textures){
        SDL_DestroyTexture(texture);
    }
}

void Cinematic::set_cinematic(PtrCinematic cinematic){
    current_frame = 0;
    for (int i = 0; i < textures.size(); i++){
        SDL_DestroyTexture(textures[i]);
    }
    textures.clear();
    this->cinematic = cinematic;
}

void Cinematic::draw_cinematic(){
    if (cinematic != nullptr){
        if (!cinematic(Camera, &current_frame, &textures)){
            current_frame = 0;
            for (int i = 0; i < textures.size(); i++){
                SDL_DestroyTexture(textures[i]);
            }
            textures.clear();
            cinematic = nullptr;
        }
    }
}