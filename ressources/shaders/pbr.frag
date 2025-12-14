#version 450

layout(location = 0) in vec2 v_uv;
layout(location = 1) in vec3 v_world_pos;
layout(location = 2) in vec3 v_world_norm;
layout(location = 3) in vec4 v_color;

layout(location = 0) out vec4 out_color;

// set=2: samplers (SDL_gpu)
layout(set = 2, binding = 0) uniform sampler2D u_albedo;   // sRGB
layout(set = 2, binding = 1) uniform sampler2D u_normal;   // linear
layout(set = 2, binding = 2) uniform sampler2D u_orm;      // linear: R=AO, G=Rough, B=Metal
layout(set = 2, binding = 3) uniform sampler2D u_emissive; // sRGB

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

// set=3: fragment UBO (SDL_gpu)
layout(std140, set = 3, binding = 0) uniform Lights
{
    vec4 ambient_light;
    vec4 directional_color_intensity;
    vec4 directional_direction;
    ivec4 counts; // x: dir_enabled, y: point_count, z: spot_count
    PointLightUBO point_lights[16];
    SpotLightUBO  spot_lights[16];

    // ✅ AJOUTE ÇA CÔTÉ C++ aussi !
    vec4 camera_world_pos; // xyz = camera position
} L;

const float PI = 3.14159265359;

vec3 srgb_to_linear(vec3 c) { return pow(c, vec3(2.2)); }
vec3 linear_to_srgb(vec3 c) { return pow(c, vec3(1.0/2.2)); }

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
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

// Normal mapping sans tangentes : TBN via dérivées
vec3 normal_from_map(vec3 N, vec3 worldPos, vec2 uv)
{
    vec3 tangentNormal = texture(u_normal, uv).xyz * 2.0 - 1.0;

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
    // --- Textures
    vec4 albedoTex = texture(u_albedo, v_uv);
    vec3 albedo = srgb_to_linear(albedoTex.rgb) * v_color.rgb; // decode sRGB
    float alpha = albedoTex.a * v_color.a;

    vec3 orm = texture(u_orm, v_uv).rgb;
    float ao        = orm.r;
    float roughness = clamp(orm.g, 0.04, 1.0);
    float metallic  = clamp(orm.b, 0.0, 1.0);

    vec3 emissive = srgb_to_linear(texture(u_emissive, v_uv).rgb);

    // --- Vectors
    vec3 N = normalize(v_world_norm);
    N = normal_from_map(N, v_world_pos, v_uv);

    vec3 V = normalize(L.camera_world_pos.xyz - v_world_pos);

    // F0
    vec3 F0 = mix(vec3(0.04), albedo, metallic);

    vec3 Lo = vec3(0.0);

    // Directional
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

        Lo += (kD * albedo / PI + spec) * radiance * NdotL;
    }

    // Point lights
    int pcount = clamp(L.counts.y, 0, 16);
    for (int i = 0; i < pcount; ++i)
    {
        vec3 toL = L.point_lights[i].position_constant.xyz - v_world_pos;
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

    // Ambient (simple)
    vec3 ambient = (L.ambient_light.rgb * L.ambient_light.a) * albedo * ao;

    vec3 color = ambient + Lo + emissive;

    // Tonemap + encode sRGB
    color = color / (color + vec3(1.0));
    color = linear_to_srgb(color);

    out_color = vec4(color, alpha);
}
