#pragma once
#include <string>
#include <vector>
#include <memory>
#include <master.hpp>
#include <ui/uicontainer.hpp>

namespace ui {
    class UI {
    public:
        UI();
        ~UI();
        void handleEvents();
        void addContainer(UIContainer container);
        
    private:
        void Draw();
        SDL_Renderer* renderer;
        SDL_Window* window;
        SDL_Event* e;
        SDL_Texture* texture;
        std::vector<UIContainer> containers;
    };
}