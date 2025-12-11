#pragma once
#include <gfx/gfx.hpp>
#include <string>
#include <vector>
namespace game
{
    struct SpriteSheet
    {
        std::shared_ptr<pixl::gfx::MaterialDesc> material;
        std::string MaterialPath;
        int numSpritesX = 1;
        int numSpritesY = 1;
    };

    struct Sprite
    {
        std::shared_ptr<SpriteSheet> sheet;
        int spriteIndex = 0;
        float posX = 0.0f;
        float posY = 0.0f;
        float scaleX = 1.0f;
        float scaleY = 1.0f;
        float rotation = 0.0f;
    };

    struct Tile
    {
        int tileIndex = 0;
        // TODO: properties
    };

    struct Tileset
    {
        std::shared_ptr<pixl::gfx::MaterialDesc> material;
        int tileWidth = 16;
        int tileHeight = 16;
        int numTilesX = 1;
        int numTilesY = 1;
        std::vector<bool> transparentTiles;
        bool isValid() const { return transparentTiles.size() == numTilesX * numTilesY; };
    };

    struct Tilemap
    {
        std::shared_ptr<Tileset> tileset;
        int numTilesX = 1;
        int numTilesY = 1;
        std::vector<Tile> tiles;
        bool isValid() const { return tiles.size() == numTilesX * numTilesY; };
    };

    struct MapDescriptor
    {
        std::string name;

        struct pos
        {
            float x = 0.0f;
            float y = 0.0f;
        };

        struct size
        {
            float width = 1.0f;
            float height = 1.0f;
        };

        std::string loadingScript;

        std::vector<std::pair<Tilemap, float>> tilemaps;
    };

    struct Map
    {
        struct pos
        {
            float x = 0.0f;
            float y = 0.0f;
        };
        struct size
        {
            float width = 1.0f;
            float height = 1.0f;
        };

        std::vector<std::pair<Tilemap, float>> tilemaps; // Tilemap et z-index
    };

}
