#version 450

layout (location = 0) out vec2 TexCoord;
layout (location = 1) flat out uint index;


struct SpritePos
{
    vec2 position;
    vec2 size;
    float rotation; // Rotation in degrees (clockwise from the center)
};

layout (set = 1, binding = 0) uniform UBO
{
    SpritePos array[440];
}spritePos;




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
    vec2 vertexPos = Vertex[gl_VertexIndex] * spritePos.array[gl_InstanceIndex].size;

    if (spritePos.array[gl_InstanceIndex].rotation != 0.0f)
    {
        
        float angle = radians(spritePos.array[gl_InstanceIndex].rotation);
        float cosAngle = cos(angle);
        float sinAngle = sin(angle);
        vertexPos = vec2(
            cosAngle * vertexPos.x - sinAngle * vertexPos.y,
            sinAngle * vertexPos.x + cosAngle * vertexPos.y
        );
    }
    vertexPos += spritePos.array[gl_InstanceIndex].position;

    gl_Position = vec4(vertexPos, 0.0, 1.0);
    TexCoord = TextureCoord[gl_VertexIndex];
    index = gl_InstanceIndex;
}