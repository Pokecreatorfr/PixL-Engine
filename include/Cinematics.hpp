#pragma once
#include <Cinematic.hpp>
#include <generated/image2cpp.hpp>


/*

Model of a cinematic function

bool cinematic_function(camera* cam, int* current_frame, std::vector<SDL_Texture*>* textures){
    if (*current_frame == 0){
        textures->push_back(Load_Texture(&image_ressource, cam->GetRenderer())); // texture 0
        textures->push_back(Load_Texture(&image_ressource, cam->GetRenderer())); // texture 1
        textures->push_back(Load_Texture(&image_ressource, cam->GetRenderer())); // texture 2
    }




    if (*current_frame < 800){ // end of the cinematic
        return false;   
    }
    (*current_frame)++;
    return true;
}

*/