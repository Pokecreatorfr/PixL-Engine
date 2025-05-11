#include <PixL-Engine.hpp>

PixL_Engine::PixL_Engine()
{
    PixL_Renderer_Init(0);
    window = CreateWindow("PixL Renderer", 800, 600, SDL_WINDOW_RESIZABLE);
    if (!window)
    {
        SDL_Log("Could not create window: %s", SDL_GetError());
        return;
    }
    PixL_Callback_WindowResized();
    PixL_2D_Init();
    PixL_Input_Handler::getInstance();
}

PixL_Engine::~PixL_Engine()
{
}

void PixL_Engine::quit()
{
    if (window)
    {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    PixL_Renderer_Quit();
    PixL_2D_Quit();
    PixL_Input_Handler::getInstance()->~PixL_Input_Handler();
}

void PixL_Engine::run()
{
    SDL_Event event;
    std::set<SDL_Keycode> keys;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_EVENT_QUIT)
        {
            this->quitRequested = true;
        }
        else if (event.type == SDL_EVENT_WINDOW_RESIZED)
        {
            PixL_Callback_WindowResized();
            PixL_2D_Callback_WindowResized();
        }

        if (event.type == SDL_EVENT_KEY_DOWN)
        {
            keys.insert(event.key.key);
        }
    }

    PixL_StartDraw();
    PixL_2D_Render();
    PixL_SwapBuffers();
}

PixL_Engine *PixL_Engine::_instance = nullptr;