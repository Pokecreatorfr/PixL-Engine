#include "tileset.hpp"

Tileset::Tileset(TilesetInfo info)
{
    this->info = info;
    this->tilesetTexture = loadTexture(info.tilesetPath.c_str(), renderer);
    this->tileCount = (tilesetTexture->w / info.tileWidth) * (tilesetTexture->h / info.tileHeight);
    this->columns = tilesetTexture->w / info.tileWidth;
    this->rows = tilesetTexture->h / info.tileHeight;
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

void Tileset::renderTile(int id, SDL_rect dest)
{
    int x = (id % columns) * info.tileWidth;
    int y = (id / columns) * info.tileHeight;
    SDL_Rect src = {x, y, info.tileWidth, info.tileHeight};
    SDL_RenderCopy(renderer, tilesetTexture, &src, &dest);
}
