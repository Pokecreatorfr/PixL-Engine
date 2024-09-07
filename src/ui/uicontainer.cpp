#include <ui/uicontainer.hpp>
#include "uicontainer.hpp"

std::vector<ui::UIContainer*> ui::UIContainer::instances_ = std::vector<ui::UIContainer*>();

ui::UIContainer::UIContainer(int w, int h, Uint32 backgroundColor)
{
    this->w = w;
    this->h = h;
    this->backgroundColor = backgroundColor;
    this->renderer = Master::getInstance()->getRenderer();
    this->window = Master::getInstance()->getWindow();
    this->e = Master::getInstance()->getEvent();
    this->texture = SDL_CreateTexture(this->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, this->w, this->h);
    SDL_SetRenderTarget(this->renderer, this->texture);
    SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 0);
    SDL_RenderClear(this->renderer);
    SDL_SetRenderTarget(this->renderer, NULL);
}




ui::UIContainer::~UIContainer()
{
}

void ui::UIContainer::handleEvents()
{   
    int mouseX, mouseY;
    bool mouseLeftButton = false;
    bool mouseRightButton = false;
    bool rightclick = false;
    bool leftclick = false;
    SDL_GetMouseState(&mouseX, &mouseY);
    if(this->e->type == SDL_MOUSEBUTTONDOWN)
    {
        if(this->e->button.button == SDL_BUTTON_LEFT)
        {
            mouseLeftButton = true;
        }
        if(this->e->button.button == SDL_BUTTON_RIGHT)
        {
            mouseRightButton = true;
        }
    }
    if(this->mouseLeftButtonPressed && !mouseLeftButton)
    {
        leftclick = true;
    }
    if(this->mouseRightButtonPressed && !mouseRightButton)
    {
        rightclick = true;
    }
    this->mouseLeftButtonPressed = mouseLeftButton;
    this->mouseRightButtonPressed = mouseRightButton;
    
    for(int i = 0 ; i < buttons.size(); i++)
    {
        if (buttons[i].button.visible)
        {
            if( isPointInRect(mouseX, mouseY, buttons[i].button.x, buttons[i].button.y, buttons[i].button.w, buttons[i].button.h) )
            {
                if(mouseLeftButton)
                {
                    buttons[i].button.state = BUTTON_STATE_CLICK;
                }
                else
                {
                    buttons[i].button.state = BUTTON_STATE_HOVER;
                }
            }
            else
            {
                buttons[i].button.state = BUTTON_STATE_NORMAL;
            }

            if(buttons[i].button.state == BUTTON_STATE_CLICK && leftclick)
            {
                buttons[i].button.color = buttons[i].buttonInfo.clickColor;
                if(buttons[i].buttonInfo.clickTexture != nullptr)
                {
                    buttons[i].button.texture = buttons[i].buttonInfo.clickTexture;
                }
                if(buttons[i].button.onClick != nullptr)
                {
                    buttons[i].button.onClick();
                }
            }
            else if(buttons[i].button.state == BUTTON_STATE_HOVER)
            {
                buttons[i].button.color = buttons[i].buttonInfo.hoverColor;
                if(buttons[i].buttonInfo.hoverTexture != nullptr)
                {
                    buttons[i].button.texture = buttons[i].buttonInfo.hoverTexture;
                }
                if(buttons[i].button.onHover != nullptr)
                {
                    buttons[i].button.onHover();
                }
            }
            else
            {
                buttons[i].button.color = buttons[i].buttonInfo.color;
                if(buttons[i].buttonInfo.texture != nullptr)
                {
                    buttons[i].button.texture = buttons[i].buttonInfo.texture;
                }
            }


            if(buttons[i].button.state != buttons[i].button.prevState)
            {
                buttons[i].button.prevState = buttons[i].button.state;
                this->needUpdate = true;
            }
        }
        if(buttons[i].button.state != buttons[i].button.prevState)
        {
            buttons[i].button.prevState = buttons[i].button.state;
            this->needUpdate = true;
        }

        

    }

    // For each list
    for(int i = 0; i < this->lists.size(); i++)
    {
        if (this->lists[i].list.visible)
        {
            for(int j = 0; j < this->lists[i].list.buttons.size(); j++)
            {
                if(lists[i].list.buttons[j].button.visible)
                {
                    if( isPointInRect(mouseX, mouseY, lists[i].list.buttons[j].button.x, lists[i].list.buttons[j].button.y, lists[i].list.buttons[j].button.w, lists[i].list.buttons[j].button.h) )
                    {
                        if(mouseLeftButton)
                        {
                            lists[i].list.buttons[j].button.state = BUTTON_STATE_CLICK;
                        }
                        else
                        {
                            lists[i].list.buttons[j].button.state = BUTTON_STATE_HOVER;
                        }
                    }
                    else
                    {
                        lists[i].list.buttons[j].button.state = BUTTON_STATE_NORMAL;
                    }

                    if(lists[i].list.buttons[j].button.state == BUTTON_STATE_CLICK && leftclick)
                    {
                        lists[i].list.buttons[j].button.color = lists[i].list.buttons[j].buttonInfo.clickColor;
                        if(lists[i].list.buttons[j].buttonInfo.clickTexture != nullptr)
                        {
                            lists[i].list.buttons[j].button.texture = lists[i].list.buttons[j].buttonInfo.clickTexture;
                        }
                        if(lists[i].list.buttons[j].button.onClick != nullptr)
                        {
                            lists[i].list.buttons[j].button.onClick();
                        }
                    }
                    else if(lists[i].list.buttons[j].button.state == BUTTON_STATE_HOVER)
                    {
                        lists[i].list.buttons[j].button.color = lists[i].list.buttons[j].buttonInfo.hoverColor;
                        if(lists[i].list.buttons[j].buttonInfo.hoverTexture != nullptr)
                        {
                            lists[i].list.buttons[j].button.texture = lists[i].list.buttons[j].buttonInfo.hoverTexture;
                        }
                        if(lists[i].list.buttons[j].button.onHover != nullptr)
                        {
                            lists[i].list.buttons[j].button.onHover();
                        }
                    }
                    else
                    {
                        lists[i].list.buttons[j].button.color = lists[i].list.buttons[j].buttonInfo.color;
                        if(lists[i].list.buttons[j].buttonInfo.texture != nullptr)
                        {
                            lists[i].list.buttons[j].button.texture = lists[i].list.buttons[j].buttonInfo.texture;
                        }
                    }

                    if(lists[i].list.buttons[j].button.state != lists[i].list.buttons[j].button.prevState)
                    {
                        lists[i].list.buttons[j].button.prevState = lists[i].list.buttons[j].button.state;
                        this->needUpdate = true;
                    }
                }
                if(lists[i].list.buttons[j].button.state != lists[i].list.buttons[j].button.prevState)
                {
                    lists[i].list.buttons[j].button.prevState = lists[i].list.buttons[j].button.state;
                    this->needUpdate = true;
                }
            }
        }
        if(this->lists[i].list.visible != this->lists[i].list.prevVisible)
        {
            this->lists[i].list.prevVisible = this->lists[i].list.visible;
            this->needUpdate = true;
        }
    }

    // For each child
    for(int i = 0; i < this->childrens.size(); i++)
    {
        this->childrens[i].handleEvents();
        if(this->childrens[i].IsNeedUpdate())
        {
            this->needUpdate = true;
        }
    }
    Draw();
}

int ui::UIContainer::getIndex()
{
    return this->index;
}

bool ui::UIContainer::isVisible()
{
    return this->visible;
}

void ui::UIContainer::setVisibility(bool visible)
{
    this->visible = visible;
}

bool ui::UIContainer::IsNeedUpdate()
{
    bool tmp = this->needUpdate;
    this->needUpdate = false;
    return tmp;
}

int ui::UIContainer::getX()
{
    return this->x;
}

int ui::UIContainer::getY()
{
    return this->y;
}

int ui::UIContainer::getW()
{
    return this->w;
}

int ui::UIContainer::getH()
{
    return this->h;
}

void ui::UIContainer::setX(int x)
{
    this->x = x;
}

void ui::UIContainer::setY(int y)
{
    this->y = y;
}

void ui::UIContainer::setW(int w)
{
    this->w = w;
    SDL_DestroyTexture(this->texture);
    this->texture = SDL_CreateTexture(this->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, this->w, this->h);
    this->needUpdate = true;
}

void ui::UIContainer::setH(int h)
{
    this->h = h;
    SDL_DestroyTexture(this->texture);
    this->texture = SDL_CreateTexture(this->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, this->w, this->h);
    this->needUpdate = true;
}

void ui::UIContainer::addChild(UIContainer child)
{
    this->childrens.push_back(child);
}

void ui::UIContainer::addButton(ButtonInfo button)
{
    BuildedButton buildedButton;
    buildedButton.buttonInfo = button;
    buildedButton.button.x = button.x;
    buildedButton.button.y = button.y;
    buildedButton.button.w = button.w;
    buildedButton.button.h = button.h;
    buildedButton.button.color = button.color;
    buildedButton.button.text = button.text;
    buildedButton.button.onClick = button.onClick;
    this->buttons.push_back(buildedButton);
    this->needUpdate = true;
}

void ui::UIContainer::addList(ListInfo list)
{
    BuildedList buildedList;
    buildedList.listInfo = list;
    buildedList.list.x = list.x;
    buildedList.list.y = list.y;
    for(int i = 0; i < list.buttons.size(); i++)
    {
        BuildedButton buildedButton;
        buildedButton.buttonInfo = list.buttons[i];
        buildedButton.button.x = list.buttons[i].x;
        buildedButton.button.y = list.buttons[i].y;
        buildedButton.button.w = list.buttons[i].w;
        buildedButton.button.h = list.buttons[i].h;
        buildedButton.button.color = list.buttons[i].color;
        buildedButton.button.text = list.buttons[i].text;
        buildedButton.button.onClick = list.buttons[i].onClick;
        buildedList.list.buttons.push_back(buildedButton);
    }
    this->needUpdate = true;
}

SDL_Texture *ui::UIContainer::getTexture()
{
    return this->texture;
}

void ui::UIContainer::Draw()
{
    if(this->visible && this->needUpdate)
    {
        SDL_SetRenderTarget(this->renderer, this->texture);
        SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 0);
        SDL_RenderClear(this->renderer);
        // draw childrens
        for(int i = 0; i < this->childrens.size(); i++)
        {
            SDL_Rect rect;
            rect.x = this->childrens[i].getX();
            rect.y = this->childrens[i].getY();
            rect.w = this->childrens[i].getW();
            rect.h = this->childrens[i].getH();
            SDL_RenderCopy(this->renderer, this->childrens[i].getTexture(), NULL, &rect);
        }
        
        // render buttons
        for(int i = 0; i < this->buttons.size(); i++)
        {
            if(this->buttons[i].button.visible)
            {
                SDL_Texture* btntexture = SDL_CreateTexture(this->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, this->buttons[i].button.w, this->buttons[i].button.h);
                SDL_SetRenderTarget(this->renderer, btntexture);
                Uint8 r, g, b, a;
                r = (this->buttons[i].button.color & 0xFF000000) >> 24;
                g = (this->buttons[i].button.color & 0x00FF0000) >> 16;
                b = (this->buttons[i].button.color & 0x0000FF00) >> 8;
                a = (this->buttons[i].button.color & 0x000000FF);
                SDL_SetRenderDrawColor(this->renderer, r, g, b, a);
                SDL_RenderClear(this->renderer);
                if(this->buttons[i].button.texture != nullptr)
                {
                    SDL_RenderCopy(this->renderer, this->buttons[i].button.texture, NULL, NULL);
                }
                SDL_SetRenderTarget(this->renderer, this->texture);
                SDL_Rect rect;
                rect.x = this->buttons[i].button.x;
                rect.y = this->buttons[i].button.y;
                rect.w = this->buttons[i].button.w;
                rect.h = this->buttons[i].button.h;
                SDL_RenderCopy(this->renderer, btntexture, NULL, &rect);
                SDL_DestroyTexture(btntexture);
            }
        }

        // render lists
        for(int i = 0; i < this->lists.size(); i++)
        {
            if(this->lists[i].list.visible)
            {
                for(int j = 0; j < this->lists[i].list.buttons.size(); j++)
                {
                    if(this->lists[i].list.buttons[j].button.visible)
                    {
                        SDL_Texture* btntexture = SDL_CreateTexture(this->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, this->lists[i].list.buttons[j].button.w, this->lists[i].list.buttons[j].button.h);
                        SDL_SetRenderTarget(this->renderer, btntexture);
                        Uint8 r, g, b, a;
                        r = (this->lists[i].list.buttons[j].button.color & 0xFF000000) >> 24;
                        g = (this->lists[i].list.buttons[j].button.color & 0x00FF0000) >> 16;
                        b = (this->lists[i].list.buttons[j].button.color & 0x0000FF00) >> 8;
                        a = (this->lists[i].list.buttons[j].button.color & 0x000000FF);
                        SDL_SetRenderDrawColor(this->renderer, r, g, b, a);
                        SDL_RenderClear(this->renderer);
                        if(this->lists[i].list.buttons[j].button.texture != nullptr)
                        {
                            SDL_RenderCopy(this->renderer, this->lists[i].list.buttons[j].button.texture, NULL, NULL);
                        }
                        SDL_SetRenderTarget(this->renderer, this->texture);
                        SDL_Rect rect;
                        rect.x = this->lists[i].list.buttons[j].button.x;
                        rect.y = this->lists[i].list.buttons[j].button.y;
                        rect.w = this->lists[i].list.buttons[j].button.w;
                        rect.h = this->lists[i].list.buttons[j].button.h;
                        SDL_RenderCopy(this->renderer, btntexture, NULL, &rect);
                        SDL_DestroyTexture(btntexture);
                    }
                }
            }
        }
    }
}
