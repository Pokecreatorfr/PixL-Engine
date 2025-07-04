#version 450

layout(location = 0) out vec4 outColor;

struct SpriteData {
    uint numTilesX;
    uint numTilesY;
    uint tileID;
    vec4 color;
    uint flags;
} ;

layout(set = 3, binding = 0) uniform UBO
{
    SpriteData array[440];
}spriteData;


layout(set = 2, binding = 0) uniform sampler2D SpriteSheet;

layout(location = 0) in vec2 TexCoord;
layout(location = 1) flat in uint index;

const uint TILE_FLAG_FLIP_X = 1u << 0;
const uint TILE_FLAG_FLIP_Y = 1u << 1;

void main()
{
    vec2 localUV = TexCoord;

    if ((spriteData.array[index].flags & TILE_FLAG_FLIP_X) != 0u) {
        localUV.x = 1.0 - localUV.x;
    }
    if ((spriteData.array[index].flags & TILE_FLAG_FLIP_Y) != 0u) {
        localUV.y = 1.0 - localUV.y;
    }

    uint tileX = spriteData.array[index].tileID % spriteData.array[index].numTilesX;
    uint tileY = spriteData.array[index].tileID / spriteData.array[index].numTilesX;
    
    vec2 tileSize = vec2(1.0 / float(spriteData.array[index].numTilesX), 1.0 / float(spriteData.array[index].numTilesY));
    vec2 tileOffset = vec2(float(tileX), float(tileY)) * tileSize;
    vec2 finalUV = tileOffset + localUV * tileSize;

    vec4 texColor = texture(SpriteSheet, finalUV);
    outColor = texColor * spriteData.array[index].color;
}
