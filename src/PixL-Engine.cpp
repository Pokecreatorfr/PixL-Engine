#include <PixL-Engine.hpp>

namespace
{
    constexpr int kDefaultWindowWidth = 1000;
    constexpr int kDefaultWindowHeight = 1000;
} // namespace

PixL_Engine::PixL_Engine()
    : window(nullptr), running(false), initialized(false), inputHandler(nullptr)
{
    PixL_Renderer_Init(0);
    window = CreateWindow("PixL Renderer", 1000, 1000, SDL_WINDOW_RESIZABLE);
    if (!window)
    {
        SDL_Log("Could not create window: %s", SDL_GetError());
        PixL_Renderer_Quit();
        quitRequested = true;
        return;
    }

    PixL_Callback_WindowResized();
    PixL_2D_Init();
    inputHandler = PixL_Input_Handler::getInstance();

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

    tilemap = std::make_unique<PixL_Tilemap>(tilemapData);

    tilemap = std::make_unique<PixL_Tilemap>(tilemapData);

    tilemap = std::make_unique<PixL_Tilemap>(tilemapData);

#ifdef PIXL_STUDIO
    PixL_Studio::getInstance()->init();
#endif

    initialized = true;
}

PixL_Engine::~PixL_Engine()
{
    quit();
    quit();
}

void PixL_Engine::quit()
{
    if (!initialized)
    {
        return;
    }

    initialized = false;

    tilemap.reset();
    sprite.reset();
    particleSystem.reset();

    if (!initialized)
    {
        return;
    }

    initialized = false;

    tilemap.reset();
    sprite.reset();
    particleSystem.reset();

    PixL_2D_Quit();
    PixL_Renderer_Quit();
    PixL_Input_Handler::destroyInstance();
    inputHandler = nullptr;
    window = nullptr;
}

void PixL_Engine::run()
{
    if (!initialized)
    {
        return;
    }

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        PixL_Imgui_ProcessEvents(&event);

        switch (event.type)
        {
        case SDL_EVENT_QUIT:
            quitRequested = true;
            break;
        case SDL_EVENT_WINDOW_RESIZED:
            PixL_Callback_WindowResized();
            PixL_2D_Callback_WindowResized();
        }

        if (event.type == SDL_EVENT_KEY_DOWN)
        {
            pressedKeys.insert(event.key.key);
        }
        else if (event.type == SDL_EVENT_KEY_UP)
        {
            pressedKeys.erase(event.key.key);
        }
    }

    if (inputHandler)
    {
        inputHandler->update(pressedKeys);
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
    if (!window)
    {
        SDL_Log("Cannot toggle fullscreen: window not created.");
        return false;
    }

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

void PixL_Engine::destroyInstance()
{
    delete _instance;
    _instance = nullptr;
}
