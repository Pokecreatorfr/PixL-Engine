#version 450


layout (location = 0) in vec2 Vertex;
layout (location = 1) in vec2 TextureCoord;
layout (location = 0) out vec2 TexCoord;


void main()
{
    gl_Position = vec4(Vertex, 0.0, 1.0);
    TexCoord = TextureCoord;
}