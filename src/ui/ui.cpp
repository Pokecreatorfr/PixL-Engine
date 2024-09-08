#include <ui/ui.hpp>


ui::UI::UI()
{
    this->renderer = Master::getInstance()->getRenderer();
    this->window = Master::getInstance()->getWindow();
    this->e = Master::getInstance()->getEvent();
    int w, h;
    SDL_GetWindowSize(this->window, &w, &h);
    this->texture = SDL_CreateTexture(this->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
}
ui::UI::~UI()
{
}

void ui::UI::handleEvents()
{

    bool needUpdate = false;
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
    SDL_SetRenderTarget(this->renderer, NULL);
    SDL_RenderCopy(this->renderer, this->texture, NULL, NULL);
}

void ui::UI::addContainer(UIContainer container)
{
    this->containers.push_back(container);
}

void ui::UI::Draw()
{
    SDL_SetRenderTarget(this->renderer, this->texture);
    SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 0);
    SDL_RenderClear(this->renderer);
    for(int i = 0; i < this->containers.size(); i++)
    {
        if(this->containers[i].isVisible())
        {
            SDL_Rect rect = {this->containers[i].getX(), this->containers[i].getY(), this->containers[i].getW(), this->containers[i].getH()};
            SDL_RenderCopy(this->renderer, this->containers[i].getTexture(), NULL, &rect);
        }
    }
}