#version 450

layout(location = 0) in vec3 vDir;
layout(location = 0) out vec4 out_color;

layout(set = 2, binding = 0) uniform samplerCube uSkybox;

void main()
{
    out_color = texture(uSkybox, normalize(vDir));
}
