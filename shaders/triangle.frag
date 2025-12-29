#version 450

layout(location = 0) in vec3 v_pos;
layout(location = 1) in vec3 v_norm;
layout(location = 2) in vec4 v_color;

layout(location = 0) out vec4 out_color;

struct PointLightUBO
{
    vec4 color_intensity;    
    vec4 position_constant;  
    vec4 attenuation;        
};

struct SpotLightUBO
{
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
} u_lights;

float lambert(vec3 n, vec3 l)
{
    return max(dot(n, l), 0.0);
}

vec3 apply_directional(vec3 n)
{
    if (u_lights.counts.x == 0) return vec3(0.0);
    vec3 l = normalize(-u_lights.directional_direction.xyz);
    float d = lambert(n, l);
    return u_lights.directional_color_intensity.rgb * (u_lights.directional_color_intensity.a * d);
}

vec3 apply_points(vec3 n)
{
    vec3 acc = vec3(0.0);
    int count = u_lights.counts.y;
    for (int i = 0; i < count; ++i)
    {
        vec3 toL = u_lights.point_lights[i].position_constant.xyz - v_pos;
        float dist = length(toL);
        vec3 l = toL / max(dist, 1e-4);

        float d = lambert(n, l);

        float c = u_lights.point_lights[i].position_constant.w;
        float lin = u_lights.point_lights[i].attenuation.x;
        float quad = u_lights.point_lights[i].attenuation.y;
        float atten = 1.0 / max(c + lin * dist + quad * dist * dist, 1e-4);

        vec3 col = u_lights.point_lights[i].color_intensity.rgb;
        float intens = u_lights.point_lights[i].color_intensity.a;

        acc += col * (intens * d * atten);
    }
    return acc;
}

vec3 apply_spots(vec3 n)
{
    vec3 acc = vec3(0.0);
    int count = u_lights.counts.z;
    for (int i = 0; i < count; ++i)
    {
        vec3 toL = u_lights.spot_lights[i].position_constant.xyz - v_pos;
        float dist = length(toL);
        vec3 l = toL / max(dist, 1e-4);

        vec3 spotDir = normalize(u_lights.spot_lights[i].direction_cutoff.xyz);
        float cosCut = u_lights.spot_lights[i].direction_cutoff.w;

        float cosAng = dot(normalize(-l), spotDir);
        if (cosAng < cosCut) continue;

        float d = lambert(n, l);

        float c = u_lights.spot_lights[i].position_constant.w;
        float lin = u_lights.spot_lights[i].attenuation.x;
        float quad = u_lights.spot_lights[i].attenuation.y;
        float atten = 1.0 / max(c + lin * dist + quad * dist * dist, 1e-4);

        vec3 col = u_lights.spot_lights[i].color_intensity.rgb;
        float intens = u_lights.spot_lights[i].color_intensity.a;

        acc += col * (intens * d * atten);
    }
    return acc;
}

void main()
{
    vec3 n = normalize(v_norm);
    vec3 amb = u_lights.ambient_light.rgb * u_lights.ambient_light.a;

    vec3 lit = amb + apply_directional(n) + apply_points(n) + apply_spots(n);
    vec3 base = v_color.rgb;

    out_color = vec4(base * lit, v_color.a);
}
