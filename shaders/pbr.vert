#version 450

layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec3 in_normal;
layout(location = 3) in vec4 in_color; 

layout(location = 0) out vec2 v_uv;
layout(location = 1) out vec3 v_pos;
layout(location = 2) out vec3 v_norm;
layout(location = 3) out vec4 v_color;

layout(std140, set = 1, binding = 0) uniform Matrices
{
    mat4 u_view;
    mat4 u_proj;
    mat4 u_model;
    mat4 u_normal;
} u_matrices;

void main()
{
    vec4 world_pos = u_matrices.u_model * vec4(in_pos, 1.0);
    v_pos = world_pos.xyz;
    v_norm = mat3(u_matrices.u_normal) * in_normal;
    v_uv = in_uv;
    v_color = in_color;
    gl_Position = u_matrices.u_proj * u_matrices.u_view * world_pos;
}
