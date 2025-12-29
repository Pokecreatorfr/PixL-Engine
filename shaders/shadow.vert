#version 450

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_uv;
layout(location = 2) in vec3 a_normal;
layout(location = 3) in vec4 a_color;

layout(std140, set = 1, binding = 0) uniform Matrices
{
    mat4 u_view;
    mat4 u_proj;
    mat4 u_model;
} M;

void main()
{
    gl_Position = M.u_proj * M.u_view * M.u_model * vec4(a_position, 1.0);
}
