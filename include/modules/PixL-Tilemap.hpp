#pragma once

#include <PixL_2D.hpp>
#include <iostream>
#include <modules/Modules-config.hpp>
#include <set>
#include <vector>

struct TilesetData
{
    std::string TextureName = "";

    // X * Y can*t be 0 and can't be greater than 65535
    uint16_t numTilesX = 0;
    uint16_t numTilesY = 0;
};

enum TileFlags
{
    TILE_FLAG_NONE = 0,
    TILE_FLIP_X = 1 << 0,
    TILE_FLIP_Y = 1 << 1,
    TILE_FLIP_XY = TILE_FLIP_X | TILE_FLIP_Y,

};

struct TileData
{
    uint16_t tileID = 0;
    uint16_t flags = 0;
};

struct TilemapData
{
    TilesetData tileset;
    uint16_t numTilesX = 0;
    uint16_t numTilesY = 0;
    std::vector<TileData> tileIDs;
    bool validate();
};

// A basic tilemap class for PixL Engine
class PixL_Tilemap
{
protected:
    uint32_t id;
    static std::set<uint32_t> tilemapIDs;

public:
    PixL_Tilemap(TilemapData tilemapData);
    ~PixL_Tilemap();

    bool updateTilemapData(std::vector<TileData> newTileIDs);

private:
    bool createTilemapTexture();
    std::string TextureName;
    TilemapData tilemapData;
};
