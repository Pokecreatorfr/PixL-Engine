#include <Render/GameRenderer.hpp>

RenderDataHandler::RenderDataHandler()
{
}

RenderDataHandler *RenderDataHandler::_instance = nullptr;

RenderDataHandler *RenderDataHandler::get_instance()
{
    if (RenderDataHandler::_instance == nullptr)
    {
        RenderDataHandler::_instance = new RenderDataHandler();
    }
    return RenderDataHandler::_instance;
}

RenderDataHandler::~RenderDataHandler()
{
    for (auto &sprite : this->spriteRenderData)
    {
        delete sprite.first;
    }
    for (auto &tilemap : this->tilemapRenderData)
    {
        delete tilemap.first;
    }
}

int RenderDataHandler::addSprite(SpriteRenderData *spriteRenderData)
{
    int id = 0;
    for (auto &sprite : this->spriteRenderData)
    {
        while (id == sprite.second)
        {
            id++;
        }
    }
    this->spriteRenderData.push_back(std::make_pair(spriteRenderData, id));
    return id;
}

void RenderDataHandler::removeSprite(int id)
{
    for (auto it = this->spriteRenderData.begin(); it != this->spriteRenderData.end(); ++it)
    {
        if (it->second == id)
        {
            this->spriteRenderData.erase(it);
            return;
        }
    }
}

void RenderDataHandler::clearSprites()
{
    this->spriteRenderData.clear();
}

int RenderDataHandler::addTilemap(TilemapRenderData *tilemapRenderData)
{
    int id = 0;
    for (auto &tilemap : this->tilemapRenderData)
    {
        while (id == tilemap.second)
        {
            id++;
        }
    }
    this->tilemapRenderData.push_back(std::make_pair(tilemapRenderData, id));
    return id;
}

void RenderDataHandler::removeTilemap(int id)
{
    for (auto it = this->tilemapRenderData.begin(); it != this->tilemapRenderData.end(); ++it)
    {
        if (it->second == id)
        {
            this->tilemapRenderData.erase(it);
            return;
        }
    }
}

void RenderDataHandler::clearTilemaps()
{
    this->tilemapRenderData.clear();
}

GameRenderer::GameRenderer(RendererWrapper *renderer)
{
    this->renderer = renderer;
    this->renderDataHandler = RenderDataHandler::get_instance();
}

GameRenderer::~GameRenderer()
{
    delete this->renderDataHandler;
}

void GameRenderer::update()
{
    // check for each api
    if (this->renderer->get_renderer_type() == RendererType::SDL2)
    {
        this->updateSDL2();
    }
}

void GameRenderer::render()
{
    // check for each api
    if (this->renderer->get_renderer_type() == RendererType::SDL2)
    {
        SDL2Renderer *sdl2Renderer = dynamic_cast<SDL2Renderer *>(this->renderer->get_renderer());
        sdl2Renderer->render();
    }
}
void GameRenderer::updateSDL2()
{
    int z = 0;
    int zmax = 0;
    for (auto &tilemap : this->renderDataHandler->getTilemapRenderData())
    {
        if (tilemap.first->z > zmax)
        {
            zmax = tilemap.first->z;
        }
    }
    for (auto &sprite : this->renderDataHandler->getSpriteRenderData())
    {
        if (sprite.first->z > zmax)
        {
            zmax = sprite.first->z;
        }
    }

    for (int i = 0; i <= zmax; i++)
    {
        for (auto &tilemap : this->renderDataHandler->getTilemapRenderData())
        {
            if (tilemap.first->z == i)
            {
                SDL2Renderer *sdl2Renderer = dynamic_cast<SDL2Renderer *>(this->renderer->get_renderer());
                TilemapRenderSDL2(sdl2Renderer, tilemap.first);
            }
        }
        for (auto &sprite : this->renderDataHandler->getSpriteRenderData())
        {
            if (sprite.first->z == i)
            {
                SDL2Renderer *sdl2Renderer = dynamic_cast<SDL2Renderer *>(this->renderer->get_renderer());
                sdl2Renderer->render();
            }
        }
    }

    SDL2Renderer *sdl2Renderer = dynamic_cast<SDL2Renderer *>(this->renderer->get_renderer());
    sdl2Renderer->update();
}

TilesetHandler::TilesetHandler()
{
    DataLoader *loader = new DataLoader(LDTK_FILE_PATH);
    quicktype::LdtkJson ldtkjs = loader->get_ldtk_json();
    std::vector<quicktype::TilesetDefinition> tilesetsDefs = ldtkjs.get_defs().get_tilesets();

    for (int i = 0; i < tilesetsDefs.size(); i++)
    {
        quicktype::TilesetDefinition tilesetDef = tilesetsDefs[i];
        Tileset tileset;
        tileset.uid = tilesetDef.get_uid();
        tileset.texturePath = tilesetDef.get_rel_path().get();
        tileset.width = tilesetDef.get_c_wid();
        tileset.height = tilesetDef.get_c_hei();
        tileset.tileWidth = tilesetDef.get_px_wid() / tilesetDef.get_c_wid();
        tileset.tileHeight = tilesetDef.get_px_hei() / tilesetDef.get_c_hei();

        this->tilesets.push_back(tileset);
    }
}

TilesetHandler *TilesetHandler::_instance = nullptr;

TilesetHandler *TilesetHandler::get_instance()
{
    if (TilesetHandler::_instance == nullptr)
    {
        TilesetHandler::_instance = new TilesetHandler();
    }
    return TilesetHandler::_instance;
}

Tileset *TilesetHandler::GetTileset(int64_t tilesetUid)
{
    for (auto &tileset : this->tilesets)
    {
        if (tileset.uid == tilesetUid)
        {
            return &tileset;
        }
    }
    return nullptr;
}

void TilemapRenderSDL2(SDL2Renderer *sdl2Renderer, TilemapRenderData *tilemap)
{
    std::string path = tilemap->tileset->texturePath;
    SDL_Texture *texture = sdl2Renderer->getTexture(path);
    int tileWidth = tilemap->tileset->tileWidth;
    int tileHeight = tilemap->tileset->tileHeight;
    int widthInTiles = tilemap->widthInTiles;

    // get screen size
    int screenWidth, screenHeight;
    SDL_GetWindowSize(sdl2Renderer->getWindow(), &screenWidth, &screenHeight);

    for (int i = 0; i < tilemap->tiles.size(); i++)
    {
        TileRenderData tile = tilemap->tiles[i];
        if (tile.alpha == 0)
        {
            continue;
        }

        int tileIndex = tile.tileIndex;
        int tilepos = tile.tilepos;

        int x = tilepos % widthInTiles;
        int y = tilepos / widthInTiles;

        SDL_Rect src = {x * tileWidth, y * tileHeight, tileWidth, tileHeight};
        // use openGL coordinates for drawing the tile

        SDL_Rect dst = {tilemap->posX + i % widthInTiles * tileWidth, tilemap->posY + i / widthInTiles * tileHeight, tileWidth, tileHeight};

        SDL_SetTextureAlphaMod(texture, tile.alpha);
        SDL_RenderCopy(sdl2Renderer->getRenderer(), texture, &src, &dst);

        SDL_SetTextureAlphaMod(texture, 255);
    }
}
