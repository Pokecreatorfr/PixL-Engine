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
    tilemapData.tileset.TextureName = "tileset1.png";
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

    PixL_CreateTexture("tileset1.png", "tileset1.png");

    this->tilemap = new PixL_Tilemap(tilemapData);

    PixL_CreateTexture("sprite.png", "sprite.png");

    SpritesheetData spritesheetData;
    spritesheetData.TextureName = "sprite.png";
    spritesheetData.numSpritesX = 4;
    spritesheetData.numSpritesY = 4;
    this->sprite = new PixL_Sprite(spritesheetData);

    this->sprite->newSprite(
        {
            {0.0f, 0.0f},
            {0.5f, 0.5f},
            SPRITE_FLAG_NONE,
            0,
            rot,
            255,
            255,
            255,
            255,
        },
        1, 0);
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
    rot += 1;
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

    sprite->updateSpriteData(0, {
                                    {0.0f, 0.0f},
                                    {0.5f, 0.5f},
                                    SPRITE_FLAG_NONE,
                                    0,
                                    rot,
                                    255,
                                    255,
                                    255,
                                    255,
                                });

    tilemap->renderTilemap(0, 0, {{0, 0}, {1, 1}});
    sprite->renderSprites();

    PixL_StartDraw();
    PixL_2D_Render();
    PixL_SwapBuffers();
}

PixL_Engine *PixL_Engine::_instance = nullptr;