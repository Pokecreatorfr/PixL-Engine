#pragma once

#include <GameLogic/InputsManager.hpp>
#include <GameLogic/Map.hpp>
#include <LoadDatas.hpp>
#include <Render/GameRenderer.hpp>

class MapHandler;

struct camera
{
    float x;
    float y;
    int h;
    int w;
    float zoom;
    float angle;
    float getRatio()
    {
        return (float)w / (float)h;
    };
};

class GameLogicHandler
{
public:
    GameLogicHandler(SystemEvent *event);
    ~GameLogicHandler();
    void update();

    int addTilemap(TilemapRenderData *tilemap);
    void removeTilemap(int id);
    void clearTilemaps();

private:
    InputsManager *inputsManager;
    DataLoader *loader;
    RenderDataHandler *renderData;

    camera camInfo = {0, 0, 0, 0};
    std::vector<std::pair<TilemapRenderData *, int>> tilemaps;
    MapHandler *mapHandler;
};