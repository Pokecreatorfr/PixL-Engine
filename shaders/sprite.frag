#version 450

layout(location = 0) out vec4 outColor;

layout(set = 3, binding = 0) uniform SpriteData {
    uint numTilesX;
    uint numTilesY;
    uint tileID;
    vec4 color;
    uint flags;
} spriteData;

layout(set = 2, binding = 0) uniform sampler2D SpriteSheet;

layout(location = 0) in vec2 TexCoord;

const uint TILE_FLAG_FLIP_X = 1u << 0;
const uint TILE_FLAG_FLIP_Y = 1u << 1;

void main()
{
    vec2 localUV = TexCoord;

    if ((spriteData.flags & TILE_FLAG_FLIP_X) != 0u) {
        localUV.x = 1.0 - localUV.x;
    }
    if ((spriteData.flags & TILE_FLAG_FLIP_Y) != 0u) {
        localUV.y = 1.0 - localUV.y;
    }

    uint tileX = spriteData.tileID % spriteData.numTilesX;
    uint tileY = spriteData.tileID / spriteData.numTilesX;
    
    vec2 tileSize = vec2(1.0 / float(spriteData.numTilesX), 1.0 / float(spriteData.numTilesY));
    vec2 tileOffset = vec2(float(tileX), float(tileY)) * tileSize;
    vec2 finalUV = tileOffset + localUV * tileSize;

    vec4 texColor = texture(SpriteSheet, finalUV);
    outColor = texColor * spriteData.color;
}
