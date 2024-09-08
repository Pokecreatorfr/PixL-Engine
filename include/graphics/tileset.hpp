#pragma once
#include <graphics/graphics.hpp>


struct TilesetInfo
{
    std::string tilesetPath;
    int tileWidth;
    int tileHeight;
};

class Tileset
{
    public:
        Tileset(TilesetInfo info);
        ~Tileset();
        TilesetInfo getTilesetInfo();
        int getTileWidth();
        int getTileHeight();
        int getTileCount();
        int getColumns();
        int getRows();
        void renderTile(int id, SDL_Rect dest);
    private:
        SDL_Texture* tilesetTexture;
        TilesetInfo info;
        int tileCount;
        int columns;
        int rows;
        SDL_Renderer* renderer;
};