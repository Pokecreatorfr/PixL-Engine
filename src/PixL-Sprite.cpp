#include <modules/PixL-Sprite.hpp>

std::set<uint32_t> PixL_Sprite::spriteIDs = {};

PixL_Sprite::PixL_Sprite(SpritesheetData spritesheetData)
{
    if ((PIXL_LOADED_MODULES & PIXL_MODULE_SPRITE) == 0)
    {
        PIXL_LOADED_MODULES |= PIXL_MODULE_SPRITE;
        std::cout << "PixL Sprite module loaded." << std::endl;

        // initialize resources for the module here
        PixL_CreatePipeline("Sprite", &sprite_vertex, &sprite_fragment, false, SDL_GPU_COMPAREOP_LESS, false, false);

        PixL_CreateUBO("SpriteVertexUBO", sizeof(spriteVertexUBO));
        PixL_CreateUBO("SpriteFragmentUBO", sizeof(spriteFragmentUBO));
    }
    this->spritesheetData = spritesheetData;
    this->TextureName = spritesheetData.TextureName;
}

uint32_t PixL_Sprite::newBatchedSprite(std::vector<SpriteUBO> spriteUBO, uint8_t layer_id, uint16_t z_index)
{
    uint32_t newID = spriteIDs.empty() ? 0 : *spriteIDs.rbegin() + 1;
    batchedSprites.insert({newID, {spriteUBO, layer_id, z_index}});
    return newID;
}

uint32_t PixL_Sprite::newSprite(SpriteUBO spriteUBO, uint8_t layer_id, uint16_t z_index)
{
    uint32_t newID = spriteIDs.empty() ? 0 : *spriteIDs.rbegin() + 1;
    spriteUBOs.insert({newID, {spriteUBO, layer_id, z_index}});
    return newID;
}

bool PixL_Sprite::updateSpriteData(uint32_t spriteID, SpriteUBO spriteUBO)
{
    auto it = spriteUBOs.find(spriteID);
    if (it != spriteUBOs.end())
    {
        it->second.spriteUBO = spriteUBO;
        return true;
    }
    std::cerr << "Sprite ID " << spriteID << " not found." << std::endl;
    return false;
}

bool PixL_Sprite::updateBatchedSpriteData(uint32_t spriteID, std::vector<SpriteUBO> spriteUBO)
{
    auto it = batchedSprites.find(spriteID);
    if (it != batchedSprites.end())
    {
        it->second.spriteUBO = spriteUBO;
        return true;
    }
    std::cerr << "Batched Sprite ID " << spriteID << " not found." << std::endl;
    return false;
}

bool PixL_Sprite::renderSprites()
{
    for (const auto &pair : spriteUBOs)
    {
        drawSprite(pair.first);
    }

    return true;
}

bool PixL_Sprite::deleteSprite(uint32_t spriteID)
{
    return false;
}

bool PixL_Sprite::deleteBatchedSprite(uint32_t spriteID)
{
    return false;
}

bool PixL_Sprite::deleteAllSprites()
{
    return false;
}

bool PixL_Sprite::drawSprite(uint32_t spriteID)
{
    auto it = spriteUBOs.find(spriteID);
    if (it == spriteUBOs.end())
    {
        std::cerr << "Sprite ID " << spriteID << " not found." << std::endl;
        return false;
    }
    SpriteData &spriteData = it->second;

    spriteVertexUBO vertexData = {spriteData.spriteUBO.position, spriteData.spriteUBO.size, spriteData.spriteUBO.rotation};
    spriteFragmentUBO fragmentData = {
        this->spritesheetData.numSpritesX, this->spritesheetData.numSpritesY, spriteData.spriteUBO.spriteTileID,
        glm::vec4(spriteData.spriteUBO.r / 255.0f, spriteData.spriteUBO.g / 255.0f, spriteData.spriteUBO.b / 255.0f, spriteData.spriteUBO.alpha / 255.0f),
        spriteData.spriteUBO.flags};

    // Utilisation d'une allocation normale mais avec vérification d'alignement
    SpriteRenderingData *spriteRenderingData = new SpriteRenderingData();
    if (!spriteRenderingData)
    {
        std::cerr << "Failed to allocate memory for SpriteRenderingData" << std::endl;
        return false;
    }

    spriteRenderingData->vertexUBO = vertexData;
    spriteRenderingData->fragmentUBO = fragmentData;
    spriteRenderingData->textureName = this->TextureName;

    PixL_2D_AddDrawable(
        spriteData.layer_id, spriteData.z_index, "Sprite",
        spriteRenderingCallback,
        static_cast<void *>(spriteRenderingData));

    return true;
}

void spriteRenderingCallback(void *data)
{
    if (!data)
    {
        std::cerr << "spriteRenderingCallback received null data" << std::endl;
        return;
    }

    SpriteRenderingData *spriteRenderingData = static_cast<SpriteRenderingData *>(data);

    try
    {
        PixL_Draw("Sprite", "", "", 1, 6, nullptr,
                  {&spriteRenderingData->vertexUBO, sizeof(spriteVertexUBO)},
                  nullptr,
                  {spriteRenderingData->textureName},
                  {&spriteRenderingData->fragmentUBO, sizeof(spriteFragmentUBO)},
                  nullptr);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception in PixL_Draw: " << e.what() << std::endl;
    }

    delete spriteRenderingData;
}