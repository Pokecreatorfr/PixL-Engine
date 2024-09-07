#include <ui/ui.hpp>
#include <ui/uicontainer.hpp>

ui::UI::UI()
{
    this->renderer = Master::getInstance()->getRenderer();
    this->window = Master::getInstance()->getWindow();
    this->e = Master::getInstance()->getEvent();
}
ui::UI::~UI()
{
}

void ui::UI::handleEvents()
{
    bool needUpdate = false;
    SDL_PollEvent(this->e);
    for(int i = 0; i < this->containers.size(); i++)
    {
        this->containers[i].handleEvents();
        if(this->containers[i].IsNeedUpdate())
        {
            needUpdate = true;
        }
    }
    if(needUpdate)
    {
        Draw();
    }

}

void ui::UI::Draw()
{
    SDL_SetRenderTarget(this->renderer, NULL);
    SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 0);
    SDL_RenderClear(this->renderer);
    SDL_RenderPresent(this->renderer);
    for(int i = 0; i < this->containers.size(); i++)
    {
        SDL_Rect rect;
        rect.x = this->containers[i].getX();
        rect.y = this->containers[i].getY();
        rect.w = this->containers[i].getW();
        rect.h = this->containers[i].getH();
        SDL_RenderCopy(this->renderer, this->containers[i].getTexture(), NULL, &rect);
    }
}