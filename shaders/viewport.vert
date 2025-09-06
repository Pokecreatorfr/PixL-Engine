#version 450

layout (location = 0) out vec2 TexCoord;

layout(set = 1, binding = 0) uniform ViewportData {
    vec2 pos;
    vec2 size;
} viewportSize;

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
    gl_Position = vec4(Vertex[gl_VertexIndex], 0.0, 1.0);
    TexCoord = TextureCoord[gl_VertexIndex] * viewportSize.size + viewportSize.pos;
}