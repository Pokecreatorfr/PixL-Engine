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
        PixL_CreatePipeline("SpriteBatch", &spriteBatch_vertex, &spriteBatch_fragment, false, SDL_GPU_COMPAREOP_LESS, false, false);

        PixL_CreateUBO("SpriteVertexUBO", sizeof(spriteVertexUBO));
        PixL_CreateUBO("SpriteFragmentUBO", sizeof(spriteFragmentUBO));
        PixL_CreateUBO("SpriteBatchVertexUBO", sizeof(spriteVertexUBO) * 440);
        PixL_CreateUBO("SpriteBatchFragmentUBO", sizeof(spriteFragmentUBO) * 440);
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

    for (const auto &pair : batchedSprites)
    {
        drawBatchedSprite(pair.first);
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

bool PixL_Sprite::drawBatchedSprite(uint32_t spriteID)
{
    auto it = batchedSprites.find(spriteID);
    if (it == batchedSprites.end())
    {
        std::cerr << "Batched Sprite ID " << spriteID << " not found." << std::endl;
        return false;
    }
    BatchedSpriteData &batchedSpriteData = it->second;

    SpriteBatchVertexUBO vertexData;
    for (const auto &sprite : batchedSpriteData.spriteUBO)
    {
        spriteVertexUBO vertex = {sprite.position, sprite.size, sprite.rotation};
        vertexData.vertexData.push_back(vertex);
    }

    SpriteBatchFragmentUBO fragmentData;
    for (const auto &sprite : batchedSpriteData.spriteUBO)
    {
        spriteFragmentUBO fragment = {
            this->spritesheetData.numSpritesX, this->spritesheetData.numSpritesY,
            sprite.spriteTileID,
            glm::vec4(sprite.r / 255.0f, sprite.g / 255.0f, sprite.b / 255.0f, sprite.alpha / 255.0f),
            sprite.flags};
        fragmentData.fragmentData.push_back(fragment);
    }

    BatchedSpriteRenderingData *batchedSpriteRenderingData = new BatchedSpriteRenderingData();
    if (!batchedSpriteRenderingData)
    {
        std::cerr << "Failed to allocate memory for BatchedSpriteRenderingData" << std::endl;
        return false;
    }

    batchedSpriteRenderingData->vertexUBO = vertexData.vertexData;
    batchedSpriteRenderingData->fragmentUBO = fragmentData.fragmentData;
    batchedSpriteRenderingData->numSprites = static_cast<uint16_t>(batchedSpriteData.spriteUBO.size());
    batchedSpriteRenderingData->textureName = this->TextureName;

    PixL_2D_AddDrawable(
        batchedSpriteData.layer_id, batchedSpriteData.z_index, "SpriteBatch",
        spriteBatchRenderingCallback,
        static_cast<void *>(batchedSpriteRenderingData));

    return true;
}

void spriteBatchRenderingCallback(void *data)
{
    if (!data)
    {
        std::cerr << "spriteBatchRenderingCallback received null data" << std::endl;
        return;
    }

    BatchedSpriteRenderingData *batchedSpriteRenderingData = static_cast<BatchedSpriteRenderingData *>(data);

    try
    {
        PixL_Draw("SpriteBatch", "", "", batchedSpriteRenderingData->numSprites, 6, nullptr,
                  {batchedSpriteRenderingData->vertexUBO.data(), sizeof(spriteVertexUBO) * batchedSpriteRenderingData->numSprites},
                  nullptr,
                  {batchedSpriteRenderingData->textureName},
                  {batchedSpriteRenderingData->fragmentUBO.data(), sizeof(spriteFragmentUBO) * batchedSpriteRenderingData->numSprites},
                  nullptr);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception in PixL_Draw: " << e.what() << std::endl;
    }

    delete batchedSpriteRenderingData;
}