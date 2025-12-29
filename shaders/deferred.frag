#version 450

layout(location = 0) in vec2 v_uv;
layout(location = 0) out vec4 out_color;

layout(set = 2, binding = 0) uniform sampler2D gPosition;
layout(set = 2, binding = 1) uniform sampler2D gNormal;
layout(set = 2, binding = 2) uniform sampler2D gAlbedo;
layout(set = 2, binding = 3) uniform sampler2D gMaterial;
layout(set = 2, binding = 4) uniform sampler2D gEmissive;
layout(set = 2, binding = 5) uniform sampler2D gEnvMap; 
layout(set = 2, binding = 6) uniform sampler2D gShadowMapDir; 
layout(set = 2, binding = 7) uniform sampler2D gShadowMapSpot; 


struct PointLightUBO {
    vec4 color_intensity;
    vec4 position_constant;
    vec4 attenuation;
};

struct SpotLightUBO {
    vec4 color_intensity;
    vec4 position_constant;
    vec4 direction_cutoff;
    vec4 attenuation;
};


layout(std140, set = 3, binding = 0) uniform Lights
{
    vec4 ambient_light;
    vec4 directional_color_intensity;
    vec4 directional_direction;
    ivec4 counts; 
    PointLightUBO point_lights[16];
    SpotLightUBO  spot_lights[16];
    vec4 camera_world_pos; 
    mat4 light_view_proj_dir; 
    mat4 light_view_proj_spot; 
    mat4 inv_view_proj; 
    vec4 screen_params; 
} L;

const float PI = 3.14159265359;
const float SHADOW_BIAS = 0.005;

vec3 linear_to_srgb(vec3 c) { return pow(c, vec3(1.0/2.2)); }
vec3 srgb_to_linear(vec3 c) { return pow(c, vec3(2.2)); }

vec3 reconstructWorldPos(vec2 uv, float depth)
{
    vec2 ndc = uv * 2.0 - 1.0;
    ndc.y = -ndc.y;
    vec4 clipPos = vec4(ndc, depth, 1.0);
    vec4 worldPos = L.inv_view_proj * clipPos;
    return worldPos.xyz / worldPos.w;
}

float calcShadow(sampler2D shadowMap, mat4 lightViewProj, vec3 worldPos, vec3 normal, vec3 lightDir)
{
    vec4 lightSpacePos = lightViewProj * vec4(worldPos, 1.0);
    vec3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
    
    projCoords.xy = projCoords.xy * 0.5 + 0.5;
    projCoords.y = 1.0 - projCoords.y;
    projCoords.z = projCoords.z;
    
    if (projCoords.x < 0.0 || projCoords.x > 1.0 || 
        projCoords.y < 0.0 || projCoords.y > 1.0 ||
        projCoords.z > 1.0 || projCoords.z < 0.0)
    {
        return 1.0;
    }
    
    float bias = max(0.01 * (1.0 - dot(normal, lightDir)), SHADOW_BIAS);
    
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += (projCoords.z - bias > pcfDepth) ? 0.0 : 1.0;
        }
    }
    shadow /= 9.0;
    
    return shadow;
}

vec3 sampleEnvMap(vec3 dir)
{
    vec3 d = dir;
    d.xz *= -1.0;
    d.z *= -1.0;
    float u = atan(d.z, d.x) / (2.0 * PI) + 0.5;
    float v = acos(clamp(d.y, -1.0, 1.0)) / PI;
    return texture(gEnvMap, vec2(u, v)).rgb;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float distributionGGX(vec3 N, vec3 H, float roughness)
{
    float a  = roughness * roughness;
    float a2 = a * a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    float denom  = (NdotH2 * (a2 - 1.0) + 1.0);
    return a2 / max(PI * denom * denom, 1e-6);
}

float geometrySchlickGGX(float NdotV, float roughness)
{
    float r = roughness + 1.0;
    float k = (r*r) / 8.0;
    return NdotV / max(NdotV * (1.0 - k) + k, 1e-6);
}

float geometrySmith(vec3 N, vec3 V, vec3 Ld, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, Ld), 0.0);
    return geometrySchlickGGX(NdotV, roughness) * geometrySchlickGGX(NdotL, roughness);
}

void main()
{
    vec3 fragPos = texture(gPosition, v_uv).xyz;
    vec3 normal = texture(gNormal, v_uv).xyz * 2.0 - 1.0; 
    vec4 albedoAlpha = texture(gAlbedo, v_uv);
    vec4 material = texture(gMaterial, v_uv);
    vec3 emissive = texture(gEmissive, v_uv).rgb; 

    vec3 albedo = albedoAlpha.rgb;
    float alpha = albedoAlpha.a;
    
    if (length(normal) < 0.1)
    {
        discard;
    }

    float metallic = material.r;
    float roughness = clamp(material.g, 0.04, 1.0); 
    float ao = material.b;

    vec3 N = normalize(normal);
    if (any(isnan(N)) || any(isinf(N)))
    {
        N = vec3(0.0, 0.0, 1.0);
    }
    vec3 V = normalize(L.camera_world_pos.xyz - fragPos);

    vec3 F0 = mix(vec3(0.04), albedo, metallic);

    vec3 Lo = vec3(0.0);

    if (L.counts.x != 0)
    {
        vec3 Ld = normalize(-L.directional_direction.xyz);
        vec3 H  = normalize(V + Ld);

        float NdotL = max(dot(N, Ld), 0.0);
        float NDF = distributionGGX(N, H, roughness);
        float G   = geometrySmith(N, V, Ld, roughness);
        vec3  F   = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 kS = F;
        vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic);

        vec3 spec = (NDF * G * F) / max(4.0 * max(dot(N, V), 0.0) * NdotL, 1e-6);

        vec3 radiance = L.directional_color_intensity.rgb * L.directional_color_intensity.a;

        float shadow = 1.0;
        if (L.counts.w != 0)
        {
            shadow = calcShadow(gShadowMapDir, L.light_view_proj_dir, fragPos, N, Ld);
        }

        Lo += (kD * albedo / PI + spec) * radiance * NdotL * shadow;
    }

    int pcount = clamp(L.counts.y, 0, 16);
    for (int i = 0; i < pcount; ++i)
    {
        vec3 toL = L.point_lights[i].position_constant.xyz - fragPos;
        float dist = length(toL);
        vec3 Ld = toL / max(dist, 1e-6);
        vec3 H  = normalize(V + Ld);

        float NdotL = max(dot(N, Ld), 0.0);

        float NDF = distributionGGX(N, H, roughness);
        float G   = geometrySmith(N, V, Ld, roughness);
        vec3  F   = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 kS = F;
        vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic);

        vec3 spec = (NDF * G * F) / max(4.0 * max(dot(N, V), 0.0) * NdotL, 1e-6);

        float c    = L.point_lights[i].position_constant.w;
        float lin  = L.point_lights[i].attenuation.x;
        float quad = L.point_lights[i].attenuation.y;
        float atten = 1.0 / max(c + lin * dist + quad * dist * dist, 1e-6);

        vec3 radiance = L.point_lights[i].color_intensity.rgb * L.point_lights[i].color_intensity.a;

        Lo += (kD * albedo / PI + spec) * radiance * NdotL * atten;
    }

    int scount = clamp(L.counts.z, 0, 16);
    for (int i = 0; i < scount; ++i)
    {
        vec3 toL = L.spot_lights[i].position_constant.xyz - fragPos;
        float dist = length(toL);
        vec3 Ld = toL / max(dist, 1e-6);
        vec3 H  = normalize(V + Ld);

        vec3 spotDir = normalize(L.spot_lights[i].direction_cutoff.xyz);
        float theta = dot(Ld, -spotDir);
        float cutoff = L.spot_lights[i].direction_cutoff.w;
        
        if (theta > cutoff)
        {
            float NdotL = max(dot(N, Ld), 0.0);

            float NDF = distributionGGX(N, H, roughness);
            float G   = geometrySmith(N, V, Ld, roughness);
            vec3  F   = fresnelSchlick(max(dot(H, V), 0.0), F0);

            vec3 kS = F;
            vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic);

            vec3 spec = (NDF * G * F) / max(4.0 * max(dot(N, V), 0.0) * NdotL, 1e-6);

            float c    = L.spot_lights[i].position_constant.w;
            float lin  = L.spot_lights[i].attenuation.x;
            float quad = L.spot_lights[i].attenuation.y;
            float atten = 1.0 / max(c + lin * dist + quad * dist * dist, 1e-6);

            float intensity = clamp((theta - cutoff) / (1.0 - cutoff), 0.0, 1.0);

            vec3 radiance = L.spot_lights[i].color_intensity.rgb * L.spot_lights[i].color_intensity.a;

            float shadow = 1.0;
            if (L.counts.w != 0 && i == 0)
            {
                shadow = calcShadow(gShadowMapSpot, L.light_view_proj_spot, fragPos, N, Ld);
            }

            Lo += (kD * albedo / PI + spec) * radiance * NdotL * atten * intensity * shadow;
        }
    }

    vec3 ambient = (L.ambient_light.rgb * L.ambient_light.a) * albedo * ao;

    vec3 ambientTotal = ambient;
    
    if (metallic > 0.01)
    {
        vec3 R = reflect(-V, N);
        vec3 envColor = srgb_to_linear(sampleEnvMap(R));
        
        vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
        
        float envIntensity = (1.0 - roughness * roughness) * metallic;
        vec3 specularIBL = envColor * F * envIntensity * ao;
        
        vec3 kD = (1.0 - F) * (1.0 - metallic);
        ambientTotal = kD * ambient + specularIBL;
    }

    vec3 color = ambientTotal + Lo + emissive;

    color = color / (color + vec3(1.0));
    color = linear_to_srgb(color);

    out_color = vec4(color, alpha);
}
