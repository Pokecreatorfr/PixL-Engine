#pragma once
#include <string>
#include <vector>
#include <SDL2/SDL.h>
#include <master.hpp>
#include <ui/button.hpp>
#include <ui/list.hpp>
#include <maths/maths.hpp>


namespace ui {

    class UIContainer
    {
    protected:
        static std::vector<UIContainer*> instances_;
    public:
        UIContainer(int x, int y ,int w, int h, Uint32 backgroundColor);
        ~UIContainer();
        void handleEvents();
        int getIndex();
        bool isVisible();
        void setVisibility(bool visible);
        bool IsNeedUpdate();
        int getX();
        int getY();
        int getW();
        int getH();
        void setX(int x);
        void setY(int y);
        void setW(int w);
        void setH(int h);
        void addChild(UIContainer child);
        void addButton(ButtonInfo button);
        void addList(ListInfo list);
        SDL_Texture* getTexture();
    private:
        int index;
        bool visible;
        bool needUpdate = true;
        void Draw();
        SDL_Renderer* renderer;
        SDL_Window* window;
        SDL_Event* e;
        int w, h;
        int x, y;
        Uint32 backgroundColor;
        SDL_Texture* texture;
        std::vector<UIContainer> childrens;
        std::vector<BuildedButton> buttons;
        std::vector<BuildedList> lists;



        bool mouseRightButtonPressed = false;
        bool mouseLeftButtonPressed = false;
    };
}