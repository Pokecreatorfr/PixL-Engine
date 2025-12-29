#version 450

layout(location = 0) in vec2 v_uv;
layout(location = 0) out vec4 out_color;

layout(set = 2, binding = 0) uniform sampler2D gPosition;
layout(set = 2, binding = 1) uniform sampler2D gNormal;
layout(set = 2, binding = 2) uniform sampler2D gAlbedo;
layout(set = 2, binding = 3) uniform sampler2D gMaterial;
layout(set = 2, binding = 4) uniform sampler2D gEmissive;
layout(set = 2, binding = 5) uniform sampler2D gDepth;

layout(std140, set = 3, binding = 0) uniform DebugParams
{
    ivec4 params;
    vec4 depth_params; 
} debug;

float linearizeDepth(float depth, float near, float far)
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main()
{
    int mode = debug.params.x;
    
    vec3 color = vec3(0.0);
    float alpha = 1.0;
    
    if (mode == 0) 
    {
        color = texture(gAlbedo, v_uv).rgb;
    }
    else if (mode == 1) 
    {
        vec3 pos = texture(gPosition, v_uv).xyz;
        color = fract(pos * 0.1);
    }
    else if (mode == 2) 
    {
        vec3 normal = texture(gNormal, v_uv).xyz;
        color = normal;
    }
    else if (mode == 3) 
    {
        color = texture(gAlbedo, v_uv).rgb;
        color = pow(color, vec3(1.0/2.2));
    }
    else if (mode == 4) 
    {
        vec4 mat = texture(gMaterial, v_uv);
        color = mat.rgb;
    }
    else if (mode == 5) 
    {
        color = texture(gEmissive, v_uv).rgb;
        color = pow(color, vec3(1.0/2.2));
    }
    else if (mode == 6) 
    {
        float depth = texture(gDepth, v_uv).r;
        float near = debug.depth_params.x;
        float far = debug.depth_params.y;
        
        float linearDepth = linearizeDepth(depth, near, far);
        float normalizedDepth = (linearDepth - near) / (far - near);
        
        color = vec3(1.0 - normalizedDepth);
    }
    
    out_color = vec4(color, alpha);
}
