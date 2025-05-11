#version 450
layout (location = 0)out vec4 outColor;

layout(set = 3 , binding = 0) uniform TilesetData
{
    uint numTilesX;
    uint numTilesY;
} tilesetData;

layout(set = 2, binding = 0) uniform sampler2D TilemapData;
layout(set = 2, binding = 1) uniform sampler2D Tileset;
layout (location = 0) in vec2 TexCoord;


const uint  TILE_FLAG_NONE = 0;
const uint  TILE_FLAG_FLIP_X = 1 << 0;
const uint  TILE_FLAG_FLIP_Y = 1 << 1;
const uint  TILE_FLAG_FLIP_XY = TILE_FLAG_FLIP_X | TILE_FLAG_FLIP_Y;

void main()
{
    // TilemapData is a 2D texture containing the tilemap data in R16G16 uint format R component is the tile index, G component contains the tile flags
    
    // discard if G component is 0
    vec4 tilemapData = texture(TilemapData, TexCoord);
    if (tilemapData.r == 0.0)
    {
        discard;
    }
    uint tileIndex = uint(tilemapData.r);
    uint tileFlags = uint(tilemapData.g);

    // Get the tile index in the tileset
    uint tileX = tileIndex % tilesetData.numTilesX;
    uint tileY = tileIndex / tilesetData.numTilesX;

    // discard if tile index is out of bounds
    if (tileX >= tilesetData.numTilesX || tileY >= tilesetData.numTilesY)
    {
        discard;
    }

    vec2 tileSize = vec2(1.0 / float(tilesetData.numTilesX), 1.0 / float(tilesetData.numTilesY));
    vec2 tilePos = vec2(float(tileX) * tileSize.x, float(tileY) * tileSize.y);

    // Get the tile color
    vec4 tileColor = texture(Tileset, tilePos + TexCoord * tileSize);
    // Apply the tile flags
    if ((tileFlags & TILE_FLAG_FLIP_X) != 0)
    {
        tileColor = texture(Tileset, tilePos + vec2(1.0 - TexCoord.x * tileSize.x, TexCoord.y * tileSize.y));
    }
    if ((tileFlags & TILE_FLAG_FLIP_Y) != 0)
    {
        tileColor = texture(Tileset, tilePos + vec2(TexCoord.x * tileSize.x, 1.0 - TexCoord.y * tileSize.y));
    }

    outColor = tileColor;
}