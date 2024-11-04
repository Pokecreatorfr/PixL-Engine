#include <GameLogic/GameLogicHandler.hpp>

GameLogicHandler::GameLogicHandler(SystemEvent *event)
{
    this->inputsManager = InputsManager::get_instance(event);
    this->renderData = RenderDataHandler::get_instance();
    this->loader = new DataLoader(LDTK_FILE_PATH);

    this->inputsManager->newKeyBind({SDL_SCANCODE_DOWN, SDL_SCANCODE_LEFT, SDL_SCANCODE_UP, SDL_SCANCODE_RIGHT});

    this->mapHandler = new MapHandler(this, &this->camInfo, &this->tilemaps, this->loader);
}

GameLogicHandler::~GameLogicHandler()
{
}

void GameLogicHandler::update()
{
    pointInt camsize = this->inputsManager->getEvent()->getWindowSize();
    this->camInfo.h = camsize.y;
    this->camInfo.w = camsize.x;

    this->inputsManager->update();

    if (this->inputsManager->getKeysPressed()[0])
    {
        this->camInfo.y -= 1;
        // std::cout << "y: " << this->camInfo.y << std::endl;
    }
    if (this->inputsManager->getKeysPressed()[1])
    {
        this->camInfo.x -= 1;
        // std::cout << "x: " << this->camInfo.x << std::endl;
    }
    if (this->inputsManager->getKeysPressed()[2])
    {
        this->camInfo.y += 1;
        // std::cout << "y: " << this->camInfo.y << std::endl;
    }
    if (this->inputsManager->getKeysPressed()[3])
    {
        this->camInfo.x += 1;
        // std::cout << "x: " << this->camInfo.x << std::endl;
    }

    this->mapHandler->update();
}

int GameLogicHandler::addTilemap(TilemapRenderData *tilemap)
{
    int id = this->renderData->addTilemap(tilemap);
    this->tilemaps.push_back({tilemap, id});
    return id;
}

void GameLogicHandler::removeTilemap(int id)
{
    this->renderData->removeTilemap(id);
    for (int i = 0; i < this->tilemaps.size(); i++)
    {
        if (this->tilemaps[i].second == id)
        {
            this->tilemaps.erase(this->tilemaps.begin() + i);
            break;
        }
    }
}

void GameLogicHandler::clearTilemaps()
{
    this->renderData->clearTilemaps();
    this->tilemaps.clear();
}