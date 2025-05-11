#version 450

layout(location = 0) out vec4 outColor;

layout(set = 3, binding = 0) uniform TilesetData {
    uint numTilesX;
    uint numTilesY;
    uint mapWidth;
    uint mapHeight;
} tilesetData;

layout(set = 2, binding = 0) uniform sampler2D TilemapData; // Tilemap , R = tileID, G = flags
layout(set = 2, binding = 1) uniform sampler2D Tileset;

layout(location = 0) in vec2 TexCoord;

const uint TILE_FLAG_FLIP_X = 1u << 0;
const uint TILE_FLAG_FLIP_Y = 1u << 1;
const float MAX_U16 = 65535.0;

void main()
{
    vec2 mapPos = TexCoord * vec2(tilesetData.mapWidth, tilesetData.mapHeight);
    ivec2 tileIdx = ivec2(floor(mapPos));
    vec2  localUV = fract(mapPos);

    uvec4 encoded = uvec4(texelFetch(TilemapData, tileIdx, 0) * MAX_U16 + 0.5);
    uint tileID    = encoded.r;
    uint tileFlags = encoded.g;

    if (tileID == 0u) {
        outColor = vec4(0.0);
        return;
    }

    uint tileX = tileID % tilesetData.numTilesX;
    uint tileY = tileID / tilesetData.numTilesX;

    vec2 tileSizeUV = vec2(1.0) / vec2(tilesetData.numTilesX, tilesetData.numTilesY);

    vec2 baseUV = vec2(tileX, tileY) * tileSizeUV;

    if ((tileFlags & TILE_FLAG_FLIP_X) != 0u) {
        localUV.x = 1.0 - localUV.x;
    }
    if ((tileFlags & TILE_FLAG_FLIP_Y) != 0u) {
        localUV.y = 1.0 - localUV.y;
    }

    vec2 finalUV = baseUV + localUV * tileSizeUV;


    outColor = texture(Tileset, finalUV);
}
