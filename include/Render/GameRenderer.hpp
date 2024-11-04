#pragma once

#include <LoadDatas.hpp>
#include <Render/RendererWrapper.hpp>
#include <config.hpp>
#include <string>
#include <utility>
#include <vector>

/*
Cette classe va interpreter les données partagées avec le GameLogicThread et les afficher à l'écran.
*/

struct SpriteSheet
{
    std::string texturePath;
    int width;
    int height;
    int spriteWidth;
    int spriteHeight;
};

struct SpriteRenderData
{
    SpriteSheet *spriteSheet;
    int spriteIndex;
    int x;
    int y;
    int z;
    int width;
    int height;
    int rotation;
    int alpha;
    bool flipX;
    bool flipY;
    bool visible;
};

struct Tileset
{
    int64_t uid;
    std::string texturePath;
    int width;
    int height;
    int tileWidth;
    int tileHeight;
};

struct TileRenderData
{
    // tiles position in the tileset
    int tileIndex;
    int tilepos;
    int alpha;
    bool flipX;
    bool flipY;
};

struct TilemapRenderData
{
    std::vector<TileRenderData> tiles;
    float posX;
    float posY;
    float width;
    float height;
    float z;
    int widthInTiles;
    Tileset *tileset;
};

class RenderDataHandler
{
protected:
    RenderDataHandler();
    static RenderDataHandler *_instance;

public:
    static RenderDataHandler *get_instance();
    ~RenderDataHandler();

    int addSprite(SpriteRenderData *spriteRenderData);
    void removeSprite(int id);
    void clearSprites();

    int addTilemap(TilemapRenderData *tilemapRenderData);
    void removeTilemap(int id);
    void clearTilemaps();

    std::vector<std::pair<SpriteRenderData *, int>> getSpriteRenderData() { return this->spriteRenderData; }
    std::vector<std::pair<TilemapRenderData *, int>> getTilemapRenderData() { return this->tilemapRenderData; }

private:
    std::vector<std::pair<SpriteRenderData *, int>> spriteRenderData;
    std::vector<std::pair<TilemapRenderData *, int>> tilemapRenderData;
};

class GameRenderer
{
public:
    GameRenderer(RendererWrapper *renderer);
    ~GameRenderer();

    void update();
    void render();

private:
    // update function for each api
    void updateSDL2();

    RendererWrapper *renderer;
    RenderDataHandler *renderDataHandler;
};

void TilemapRenderSDL2(SDL2Renderer *sdl2Renderer, TilemapRenderData *tilemap);

class TilesetHandler
{
protected:
    TilesetHandler();
    static TilesetHandler *_instance;

public:
    static TilesetHandler *get_instance();

    Tileset *GetTileset(int64_t tilesetUid);

private:
    std::vector<Tileset> tilesets;
};