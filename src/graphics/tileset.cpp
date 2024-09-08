#include <graphics/tileset.hpp>

Tileset::Tileset(TilesetInfo info)
{
    this->renderer = Master::getInstance()->getRenderer();
    this->info = info;
    this->tilesetTexture = loadTexture(info.tilesetPath.c_str(), this->renderer);
}

Tileset::~Tileset()
{
    SDL_DestroyTexture(tilesetTexture);
}

TilesetInfo Tileset::getTilesetInfo()
{
    return info;
}

int Tileset::getTileWidth()
{
    return info.tileWidth;
}

int Tileset::getTileHeight()
{
    return info.tileHeight;
}

int Tileset::getTileCount()
{
    return tileCount;
}

int Tileset::getColumns()
{
    return columns;
}

int Tileset::getRows()
{
    return rows;
}

void Tileset::renderTile(int id, SDL_Rect dest)
{
    int x = (id % columns) * info.tileWidth;
    int y = (id / columns) * info.tileHeight;
    SDL_Rect src = {x, y, info.tileWidth, info.tileHeight};
    SDL_RenderCopy(renderer, tilesetTexture, &src, &dest);
}
