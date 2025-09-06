#include <PixL-Engine.hpp>

PixL_Engine::PixL_Engine()
{

    PixL_Renderer_Init(0);
    window = CreateWindow("PixL Renderer", 1000, 1000, SDL_WINDOW_RESIZABLE);
    if (!window)
    {
        SDL_Log("Could not create window: %s", SDL_GetError());
        return;
    }

    PixL_Callback_WindowResized();
    PixL_2D_Init();
    PixL_Input_Handler::getInstance();

    TilemapData tilemapData;
    tilemapData.tileset.TextureName = "tileset1";
    tilemapData.tileset.numTilesX = 36;
    tilemapData.tileset.numTilesY = 14;
    tilemapData.numTilesX = 3;
    tilemapData.numTilesY = 2;
    tilemapData.tileIDs.resize(tilemapData.numTilesX * tilemapData.numTilesY);
    tilemapData.tileIDs[0].tileID = 37;
    tilemapData.tileIDs[0].flags = TILE_FLAG_NONE;
    tilemapData.tileIDs[1].tileID = 38;
    tilemapData.tileIDs[1].flags = TILE_FLAG_NONE;
    tilemapData.tileIDs[2].tileID = 39;
    tilemapData.tileIDs[2].flags = TILE_FLAG_NONE;
    tilemapData.tileIDs[3].tileID = 73;
    tilemapData.tileIDs[3].flags = TILE_FLAG_NONE;
    tilemapData.tileIDs[4].tileID = 74;
    tilemapData.tileIDs[4].flags = TILE_FLAG_NONE;
    tilemapData.tileIDs[5].tileID = 75;
    tilemapData.tileIDs[5].flags = TILE_FLAG_NONE;

    PixL_Ressource::getInstance()->loadPak("assets.pak");

#ifdef PIXL_STUDIO
    PixL_Studio::getInstance()->init();
#endif
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
        PixL_Imgui_ProcessEvents(&event);
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

#ifdef PIXL_STUDIO
    PixL_Studio::getInstance()->update();
#endif

    PixL_StartDraw();
    PixL_2D_Render();
    PixL_Imgui_Update_Callbacks();
    PixL_SwapBuffers();
}

bool PixL_Engine::ToogleFullscreen()
{
    Uint32 flags = SDL_GetWindowFlags(window);
    if (flags & SDL_WINDOW_FULLSCREEN)
    {
        if (SDL_SetWindowFullscreen(window, false) != 0)
        {
            SDL_Log("Could not switch to windowed mode: %s", SDL_GetError());
            return false;
        }
    }
    else
    {
        if (SDL_SetWindowFullscreen(window, true) != 0)
        {
            SDL_Log("Could not switch to fullscreen mode: %s", SDL_GetError());
            return false;
        }
    }
    return true;
}

PixL_Engine *PixL_Engine::_instance = nullptr;