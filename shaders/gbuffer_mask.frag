#version 450

layout(location = 0) in vec2 v_uv;
layout(location = 1) in vec3 v_world_pos;
layout(location = 2) in vec3 v_world_norm;
layout(location = 3) in vec4 v_color;


layout(location = 0) out vec4 gPosition;   
layout(location = 1) out vec4 gNormal;      
layout(location = 2) out vec4 gAlbedo;      
layout(location = 3) out vec4 gMaterial;   
layout(location = 4) out vec4 gEmissive;   

layout(set = 2, binding = 0) uniform sampler2D u_albedo;   
layout(set = 2, binding = 1) uniform sampler2D u_normal;   
layout(set = 2, binding = 2) uniform sampler2D u_orm;      
layout(set = 2, binding = 3) uniform sampler2D u_emissive; 

layout(std140, set = 3, binding = 0) uniform MaterialFactors
{
    vec4 factors;  
    ivec4 flags;   
    ivec4 flags2; 
} u_material;

vec3 srgb_to_linear(vec3 c) { return pow(c, vec3(2.2)); }

vec3 normal_from_map(vec3 N, vec3 worldPos, vec2 uv)
{
    vec3 tangentNormal = texture(u_normal, uv).xyz * 2.0 - 1.0;
    tangentNormal.y = -tangentNormal.y;

    vec3 dp1 = dFdx(worldPos);
    vec3 dp2 = dFdy(worldPos);
    vec2 duv1 = dFdx(uv);
    vec2 duv2 = dFdy(uv);

    vec3 T = normalize(dp1 * duv2.y - dp2 * duv1.y);
    vec3 B = normalize(-dp1 * duv2.x + dp2 * duv1.x);

    mat3 TBN = mat3(T, B, N);
    return normalize(TBN * tangentNormal);
}

void main()
{
    vec4 albedoTex = texture(u_albedo, v_uv);
    float alpha = albedoTex.a * v_color.a;
    
    if (alpha < 0.5)
        discard;

    gPosition = vec4(v_world_pos, 1.0);

    vec3 N = normalize(v_world_norm);
    if (u_material.flags2.x != 0)
    {
        N = normal_from_map(N, v_world_pos, v_uv);
    }
    if (any(isnan(N)) || any(isinf(N)) || length(N) < 0.5)
    {
        N = normalize(v_world_norm);
    }
    gNormal = vec4(N * 0.5 + 0.5, 1.0); 

    vec3 albedo = srgb_to_linear(albedoTex.rgb) * v_color.rgb;
    gAlbedo = vec4(albedo, 1.0); 

    vec3 orm = texture(u_orm, v_uv).rgb;
    float ao = (u_material.flags.z != 0) ? orm.r : u_material.factors.z;
    float roughness = (u_material.flags.y != 0) ? clamp(orm.g, 0.04, 1.0) : u_material.factors.y;
    float metallic = (u_material.flags.y != 0) ? clamp(orm.b, 0.0, 1.0) : u_material.factors.x;

    gMaterial = vec4(metallic, roughness, ao, 1.0);

    vec3 emissive = vec3(0.0);
    if (u_material.flags.w != 0)
    {
        emissive = srgb_to_linear(texture(u_emissive, v_uv).rgb) * v_color.rgb * u_material.factors.w;
    }
    gEmissive = vec4(emissive, 1.0);
}
