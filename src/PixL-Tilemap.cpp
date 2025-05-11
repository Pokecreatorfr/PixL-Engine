#include <modules/PixL-Tilemap.hpp>

bool TilemapData::validate()
{
    if (tileset.TextureName.empty())
    {
        std::cerr << "Tileset name is empty." << std::endl;
        return false;
    }
    if (tileset.numTilesX == 0 || tileset.numTilesY == 0 || tileset.numTilesX * tileset.numTilesY > 65535)
    {
        std::cerr << "Tileset dimensions are invalid." << std::endl;
        return false;
    }
    if (numTilesX == 0 || numTilesY == 0)
    {
        std::cerr << "Tilemap dimensions are invalid." << std::endl;
        return false;
    }
    if (tileIDs.size() != numTilesX * numTilesY)
    {
        std::cerr << "Tile IDs size does not match tilemap dimensions." << std::endl;
        return false;
    }
    return true;
}

PixL_Tilemap::PixL_Tilemap(TilemapData tilemapData)
{
    if (!PIXL_LOADED_MODULES & PIXL_MODULE_TILEMAP)
    {
        PIXL_LOADED_MODULES |= PIXL_MODULE_TILEMAP;
        std::cout << "PixL Tilemap module loaded." << std::endl;

        // initialize ressources for the module here
    }

    if (tilemapIDs.empty())
    {
        id = 0;
    }
    else
    {
        id = *tilemapIDs.rbegin() + 1;
    }
    if (tilemapData.validate())
    {
        this->tilemapData = tilemapData;
    }
    else
    {
        std::cerr << "Tilemap data validation failed." << std::endl;
        return;
    }
    if (!createTilemapTexture())
    {
        std::cerr << "Failed to create tilemap texture." << std::endl;
        return;
    }

    if (!PixL_UpdateTexture(TextureName, tilemapData.tileIDs.data(),
                            tilemapData.numTilesX * tilemapData.numTilesY * sizeof(uint32_t)))
    {
        std::cerr << "Failed to update tilemap texture." << std::endl;
        return;
    }
}

std::set<uint32_t> PixL_Tilemap::tilemapIDs = {};

PixL_Tilemap::~PixL_Tilemap()
{
}

bool PixL_Tilemap::updateTilemapData(std::vector<TileData> newTileIDs)
{
    if (newTileIDs.size() != tilemapData.numTilesX * tilemapData.numTilesY)
    {
        std::cerr << "New tile IDs size does not match tilemap dimensions." << std::endl;
        return false;
    }
    tilemapData.tileIDs = newTileIDs;

    if (!PixL_UpdateTexture(TextureName, tilemapData.tileIDs.data(),
                            tilemapData.numTilesX * tilemapData.numTilesY * sizeof(TileData) * 2))
    {
        std::cerr << "Failed to update tilemap texture." << std::endl;
        return false;
    }
    return true;
}

bool PixL_Tilemap::createTilemapTexture()
{
    std::string textureName = "tilemap_" + std::to_string(id);
    if (PixL_CreateBlankTexture(textureName, tilemapData.numTilesX, tilemapData.numTilesY,
                                SDL_GPU_TEXTUREUSAGE_SAMPLER, SDL_GPU_TEXTUREFORMAT_R16G16_UINT))
    {
        TextureName = textureName;
        return true;
    }
    throw std::runtime_error("Failed to create tilemap texture.");
    return false;
}
