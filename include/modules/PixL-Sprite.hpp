#pragma once

#include <PixL_2D.hpp>
#include <iostream>
#include <modules/Modules-config.hpp>
#include <set>
#include <vector>

struct SpritesheetData
{
    std::string TextureName = "";
    uint16_t numSpritesX = 0;
    uint16_t numSpritesY = 0;
};

enum SpriteFlags
{
    SPRITE_FLAG_NONE = 0,
    SPRITE_FLIP_X = 1 << 0,
    SPRITE_FLIP_Y = 1 << 1,
    SPRITE_FLIP_XY = SPRITE_FLIP_X | SPRITE_FLIP_Y,
};

struct alignas(16) SpriteUBO
{
    glm::vec2 position = {0.0f, 0.0f};
    glm::vec2 size = {1.0f, 1.0f};
    SpriteFlags flags = SPRITE_FLAG_NONE;
    uint16_t spriteTileID = 0; // ID of the sprite in the spritesheet
    float rotation = 0.0f;     // Rotation in degrees (clockwise from the center)
    uint8_t alpha = 255;       // Alpha value for transparency
    uint8_t r = 255;           // Red component of color
    uint8_t g = 255;           // Green component of color
    uint8_t b = 255;           // Blue component of color
};

alignas(16) struct spriteVertexUBO
{
    alignas(8) glm::vec2 position = {0.0f, 0.0f};
    alignas(8) glm::vec2 size = {1.0f, 1.0f};
    alignas(4) float rotation = 0.0f; // Rotation in degrees ( clockwise )

    alignas(4) float _pad0; // offset 20
    alignas(4) float _pad1; // offset 24
    alignas(4) float _pad2; // offset 28
};

alignas(16) struct spriteFragmentUBO
{
    alignas(4) uint32_t numTilesX = 0; // Number of tiles in the X direction
    alignas(4) uint32_t numTilesY = 0; // Number of tiles in the Y direction
    alignas(4) uint32_t tileID = 0;    // ID of the sprite in the spritesheet
    alignas(16) glm::vec4 color = {1.0f, 1.0f, 1.0f, 1.0f};
    alignas(4) uint32_t flags = SPRITE_FLAG_NONE; // Flags for sprite rendering (e.g., flip)
};

struct SpriteData
{
    SpriteUBO spriteUBO = {};
    uint8_t layer_id = 0; // Layer ID for rendering
    uint16_t z_index = 0; // Priority for rendering order
};

struct SpriteBatchVertexUBO
{
    std::vector<spriteVertexUBO> vertexData;
};

struct SpriteBatchFragmentUBO
{
    std::vector<spriteFragmentUBO> fragmentData;
};

struct BatchedSpriteData
{
    std::vector<SpriteUBO> spriteUBO = {};
    uint8_t layer_id = 0; // Layer ID for rendering
    uint16_t z_index = 0; // Priority for rendering order
};

struct SpriteRenderingData
{
    spriteVertexUBO vertexUBO = {};
    spriteFragmentUBO fragmentUBO = {};
    std::string textureName = ""; // Name of the texture used for the sprite
};

struct BatchedSpriteRenderingData
{
    std::vector<spriteVertexUBO> vertexUBO = {};
    std::vector<spriteFragmentUBO> fragmentUBO = {};
    uint16_t numSprites = 0;      // Number of sprites in the batch
    std::string textureName = ""; // Name of the texture used for the batched sprite
};

class PixL_Sprite
{
    uint32_t id;
    static std::set<uint32_t> spriteIDs;

public:
    PixL_Sprite(SpritesheetData spritesheetData);
    ~PixL_Sprite();

    uint32_t newBatchedSprite(std::vector<SpriteUBO> spriteUBO, uint8_t layer_id = 0, uint16_t z_index = 0);
    uint32_t newSprite(SpriteUBO spriteUBO, uint8_t layer_id = 0, uint16_t z_index = 0);

    bool updateSpriteData(uint32_t spriteID, SpriteUBO spriteUBO);
    bool updateBatchedSpriteData(uint32_t spriteID, std::vector<SpriteUBO> spriteUBO);

    bool renderSprites();

    bool deleteSprite(uint32_t spriteID);
    bool deleteBatchedSprite(uint32_t spriteID);
    bool deleteAllSprites();

protected:
    SpritesheetData spritesheetData;
    std::string TextureName;

    std::map<uint32_t, BatchedSpriteData> batchedSprites = {};
    std::map<uint32_t, SpriteData> spriteUBOs = {};

    bool createSpriteTexture();
    bool drawSprite(uint32_t spriteID);
    bool drawBatchedSprite(uint32_t spriteID);
};

void spriteRenderingCallback(void *data);
void spriteBatchRenderingCallback(void *data);