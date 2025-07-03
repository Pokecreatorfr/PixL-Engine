#version 450

layout (location = 0) out vec2 TexCoord;

layout (set = 1, binding = 0) uniform SpritePos
{
    vec2 position;
    vec2 size;
    float rotation; // Rotation in degrees (clockwise from the center)
} spritePos;

const vec2 Vertex[6] = vec2[6](
    vec2(-1.0, -1.0),
    vec2( 1.0, -1.0),
    vec2(-1.0,  1.0),
    vec2( 1.0,  1.0),
    vec2(-1.0,  1.0),
    vec2( 1.0, -1.0)
);

const vec2 TextureCoord[6] = vec2[6](
    vec2(0.0, 1.0),
    vec2(1.0, 1.0),
    vec2(0.0, 0.0),
    vec2(1.0, 0.0),
    vec2(0.0, 0.0),
    vec2(1.0, 1.0)
);

void main()
{
    vec2 vertexPos = Vertex[gl_VertexIndex] * spritePos.size;

    if (spritePos.rotation != 0.0f)
    {
        
        float angle = radians(spritePos.rotation);
        float cosAngle = cos(angle);
        float sinAngle = sin(angle);
        vertexPos = vec2(
            cosAngle * vertexPos.x - sinAngle * vertexPos.y,
            sinAngle * vertexPos.x + cosAngle * vertexPos.y
        );
    }
    vertexPos += spritePos.position;

    gl_Position = vec4(vertexPos, 0.0, 1.0);
    TexCoord = TextureCoord[gl_VertexIndex];
}