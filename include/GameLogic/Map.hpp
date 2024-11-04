#pragma once
#include <GameLogic/GameLogicHandler.hpp>
#include <LoadDatas.hpp>
#include <Maths/Geometry.hpp>
#include <Render/GameRenderer.hpp>
#include <config.hpp>

class GameLogicHandler;
class TilemapRenderData;
struct camera;
class Map;

class MapHandler
{
public:
    MapHandler(GameLogicHandler *gameLogicHandler, camera *camInfo, std::vector<std::pair<TilemapRenderData *, int>> *tilemaps, DataLoader *loader);
    ~MapHandler();
    void update();

private:
    camera *camInfo;
    std::vector<std::pair<TilemapRenderData *, int>> *tilemaps;
    GameLogicHandler *gameLogicHandler;
    std::vector<Map *> maps;
    std::vector<quicktype::Level> levels;
};

class MapLayer
{
public:
    MapLayer(quicktype::LayerInstance layerInstance, quicktype::Level level, camera *camInfo, std::pair<TilemapRenderData *, int> *tilemap);
    ~MapLayer();
    void update();
    int getLayerId();
    quicktype::LayerInstance getLayerInstance();

private:
    std::pair<TilemapRenderData *, int> *tilemap;
    camera *camInfo;
    quicktype::LayerInstance layerInstance;
    quicktype::Level level;
    int layerId;
};

class Map
{
public:
    Map(std::vector<std::pair<TilemapRenderData *, int> *> tilemap, camera *camInfo, quicktype::Level level);
    ~Map();
    void update();
    int getLevelId();
    quicktype::Level getLevel();
    std::vector<int> getTilemapsId();

private:
    std::vector<std::pair<TilemapRenderData *, int> *> tilemap;
    camera *camInfo;
    int levelId;
    quicktype::Level level;
    std::vector<MapLayer *> layers;
};
