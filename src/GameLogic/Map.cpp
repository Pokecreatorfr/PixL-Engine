#include <GameLogic/Map.hpp>

MapHandler::MapHandler(GameLogicHandler *gameLogicHandler, camera *camInfo, std::vector<std::pair<TilemapRenderData *, int>> *tilemaps, DataLoader *loader)
{
    this->camInfo = camInfo;
    this->tilemaps = tilemaps;
    this->gameLogicHandler = gameLogicHandler;
    this->levels = loader->get_ldtk_json().get_levels();
}

MapHandler::~MapHandler()
{
}

void MapHandler::update()
{
    // check new levels to add
    for (int i = 0; i < this->levels.size(); i++)
    {
        SDL_Rect rect = {this->levels[i].get_world_x(), this->levels[i].get_world_y(), this->levels[i].get_px_wid(), this->levels[i].get_px_hei()};
        if (checkRectVisible(this->camInfo, &rect))
        {
            bool alreadyExist = false;
            for (int i = 0; i < this->maps.size(); i++)
            {
                if (this->maps[i]->getLevelId() == this->levels[i].get_uid())
                {
                    alreadyExist = true;
                    break;
                }
            }
            if (!alreadyExist)
            {
                std::vector<std::pair<TilemapRenderData *, int> *> tilemapsvector;

                int tilemapSize = this->levels[i].get_layer_instances().get().size();

                std::cout << "Level " << this->levels[i].get_layer_instances().get().size() << std::endl;

                for (int i = 0; i < tilemapSize; i++)
                {
                    TilemapRenderData *tilemap = new TilemapRenderData();
                    int id = this->gameLogicHandler->addTilemap(tilemap);
                    std::pair<TilemapRenderData *, int> *pair = new std::pair<TilemapRenderData *, int>(tilemap, id);
                    tilemapsvector.push_back(pair);
                    this->tilemaps->push_back(*pair);
                }

                std::cout << "Tilemaps size: " << tilemapsvector.size() << std::endl;

                Map *map = new Map(tilemapsvector, this->camInfo, this->levels[i]);
                this->maps.push_back(map);
            }
        }
    }

    // check levels to remove

    for (int i = 0; i < this->maps.size(); i++)
    {
        SDL_Rect rect = {this->maps[i]->getLevel().get_world_x(), this->maps[i]->getLevel().get_world_y(), this->maps[i]->getLevel().get_px_wid(), this->maps[i]->getLevel().get_px_hei()};
        if (!checkRectVisible(this->camInfo, &rect))
        {
            std::vector<int> tilemapsId = this->maps[i]->getTilemapsId();
            for (int i = 0; i < tilemapsId.size(); i++)
            {
                this->gameLogicHandler->removeTilemap(tilemapsId[i]);
            }
            this->maps[i]->~Map();
            this->maps.erase(this->maps.begin() + i);
            i--;
        }
    }

    for (int i = 0; i < this->maps.size(); i++)
    {
        this->maps[i]->update();
    }
}

Map::Map(std::vector<std::pair<TilemapRenderData *, int> *> tilemap, camera *camInfo, quicktype::Level level)
{
    this->tilemap = tilemap;
    this->camInfo = camInfo;
    this->levelId = level.get_uid();
    this->level = level;
    std::cout << "Map created with level id: " << this->levelId << std::endl;

    std::cout << "Tilemap size: " << level.get_layer_instances().get().size() << std::endl;

    // create layers
    for (int i = 0; i < tilemap.size(); i++)
    {
        MapLayer *layer = new MapLayer(level.get_layer_instances().get().data()[i], level, camInfo, tilemap[i]);
        this->layers.push_back(layer);
    }
}

Map::~Map()
{
    for (int i = 0; i < this->layers.size(); i++)
    {
        this->layers[i]->~MapLayer();
    }
    std::cout << "Map destroyed with level id: " << this->levelId << std::endl;
}

void Map::update()
{
    for (int i = 0; i < this->layers.size(); i++)
    {
        this->layers[i]->update();
    }
}

int Map::getLevelId()
{
    return this->levelId;
}

quicktype::Level Map::getLevel()
{
    return this->level;
}

std::vector<int> Map::getTilemapsId()
{
    std::vector<int> ids;
    for (int i = 0; i < this->tilemap.size(); i++)
    {
        ids.push_back(this->tilemap[i]->second);
    }
    return ids;
}

MapLayer::MapLayer(quicktype::LayerInstance layerInstance, quicktype::Level level, camera *camInfo, std::pair<TilemapRenderData *, int> *tilemap)
{
    this->tilemap = tilemap;
    this->layerInstance = layerInstance;
    this->level = level;
    this->camInfo = camInfo;
    this->layerId = layerInstance.get_layer_def_uid();
    std::cout << "Layer created with id: " << this->layerId << std::endl;
    this->tilemap->first->tileset = TilesetHandler::get_instance()->GetTileset(layerInstance.get_tileset_def_uid().get());

    this->tilemap->first->widthInTiles = this->level.get_layer_instances().get().data()->get_c_wid();

    int tilepxWidth = tilemap->first->tileset->tileWidth;
    int tilepxHeight = tilemap->first->tileset->tileHeight;
    for (int i = 0; i < this->layerInstance.get_grid_tiles().size(); i++)
    {
        TileRenderData tile;
        quicktype::TileInstance tileInstance = this->layerInstance.get_grid_tiles().data()[i];
        int f = tileInstance.get_f();
        if (f == 0)
        {
            tile.flipX = false;
            tile.flipY = false;
        }
        else if (f == 1)
        {
            tile.flipX = true;
            tile.flipY = false;
        }
        else if (f == 2)
        {
            tile.flipX = false;
            tile.flipY = true;
        }
        else if (f == 3)
        {
            tile.flipX = true;
            tile.flipY = true;
        }

        float a = tileInstance.get_a();
        tile.alpha = a * 255;

        tile.tileIndex = tileInstance.get_t();

        int x = tileInstance.get_px()[0];
        int y = tileInstance.get_px()[1];

        int tilepos = (x / tilepxWidth) + (y / tilepxHeight) * tilemap->first->widthInTiles;

        tile.tilepos = tilepos;
    }
}

MapLayer::~MapLayer()
{
}

void MapLayer::update()
{
    // 1 tile = 0.1f in OpenGl

    float camRatio = this->camInfo->getRatio();

    float camPosx = this->camInfo->x;
    float camPosy = this->camInfo->y;

    float camZoom = this->camInfo->zoom;

    float camRot = this->camInfo->angle;

    float levelPosX = this->level.get_world_x();
    float levelPosY = this->level.get_world_y();

    // campos is the center of the screen
    // level position is the top left corner of the level
    // take zoom into account
    float tilemapPosX = ((levelPosX * camZoom / TILE_SIZE) - camPosx) * 0.1f;
    float tilemapPosY = ((levelPosY * camZoom / TILE_SIZE) - camPosy) * 0.1f * camRatio;

    // tilemap size
    float tilemapWidth = this->level.get_layer_instances().get().data()->get_c_wid() * 0.1f * camZoom;
    float tilemapHeight = this->level.get_layer_instances().get().data()->get_c_hei() * 0.1f * camRatio * camZoom;

    this->tilemap->first->posX = tilemapPosX;
    this->tilemap->first->posY = tilemapPosY;
    this->tilemap->first->width = tilemapWidth;
    this->tilemap->first->height = tilemapHeight;
}

int MapLayer::getLayerId()
{
    return this->layerId;
}

quicktype::LayerInstance MapLayer::getLayerInstance()
{
    return this->layerInstance;
}
