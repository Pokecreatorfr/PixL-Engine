#version 450

layout(location = 0) in vec3 aPos;

layout(location = 0) out vec3 vDir;

// Vertex shader uniform buffers = set 1
layout(set = 1, binding = 0, std140) uniform CameraUBO
{
    mat4 uProj;
    mat4 uView; // view matrix (caméra)
} cam;

void main()
{
    // Ignore la translation : skybox collée à la caméra
    mat4 viewRotOnly = mat4(mat3(cam.uView));

    vDir = mat3(viewRotOnly) * aPos;

    vec4 pos = cam.uProj * viewRotOnly * vec4(aPos, 1.0);

    // Force la profondeur au fond
    gl_Position = pos.xyww;
}
