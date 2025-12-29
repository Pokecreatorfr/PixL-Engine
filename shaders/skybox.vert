#version 450

layout(location = 0) in vec3 aPos;

layout(location = 0) out vec3 vDir;


layout(set = 1, binding = 0, std140) uniform CameraUBO
{
    mat4 uProj;
    mat4 uView; 
} cam;

void main()
{
    
    mat4 viewRotOnly = mat4(mat3(cam.uView));

    vDir = mat3(viewRotOnly) * aPos;

    vec4 pos = cam.uProj * viewRotOnly * vec4(aPos, 1.0);

    
    gl_Position = pos.xyww;
}
