#include <GuiRenderer.hpp>


GuiRenderer::GuiRenderer(){
    Camera = Camera::GetInstance();
}

GuiRenderer* GuiRenderer::instance_ = nullptr;

GuiRenderer* GuiRenderer::GetInstance(){
    if (instance_ == nullptr){
        instance_ = new GuiRenderer();
    }
    return instance_;
}

GuiRenderer::~GuiRenderer(){
    for (int i = 0; i < elements.size(); i++){
        for (int x = 0; x < elements[i]->textures.size(); x++){
            SDL_DestroyTexture(elements[i]->textures[x]);
        }
        elements[i]->textures.clear();
    }
    elements.clear();
}

void GuiRenderer::draw_gui(){
    int count = 0;
    int priority = 0;
    while (count < elements.size()){
        for (int i = 0; i < elements.size(); i++){
            if (elements[i]->priority == priority){
                elements[i]->PtrGui(Camera, elements[i]->w, elements[i]->h, elements[i]->x, elements[i]->y, &elements[i]->textures, elements[i]->font_renderer, elements[i]->param);
                count++;
            }
        }
        priority++;
    }
}

int GuiRenderer::add_gui_element(gui_element* element){
    if (elements.size() == 0){
        element->uid = 0;
    }
    else
    {
        int x = 0;
        bool found = false;
        while (!found){
            found = true;
            for (int i = 0; i < elements.size(); i++){
                if (elements[i]->uid == x){
                    found = false;
                    x++;
                    break;
                }
            }
        }
        element->uid = x;
    }
    elements.push_back(element);
    return element->uid;
}

void GuiRenderer::remove_gui_element(int uid){
    for (int i = 0; i < elements.size(); i++){
        if (elements[i]->uid == uid){
            for (int x = 0; x < elements[i]->textures.size(); x++){
                SDL_DestroyTexture(elements[i]->textures[x]);
            }
            elements[i]->textures.clear();
            elements.erase(elements.begin() + i);
            return;
        }
    }
}

