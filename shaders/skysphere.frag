#version 450

layout(location = 0) in vec2 vUV;   // doit venir du vertex fullscreen (0..1)
layout(location = 0) out vec4 out_color;

layout(set = 3, binding = 0, std140) uniform CameraUBO {
    mat4 uProj;  // projection normale
    mat4 uView;  // view normale
} cam;

layout(set = 2, binding = 0) uniform sampler2D uSkyTex;

const float PI = 3.14159265358979323846;

void main()
{
    // 1) UV -> NDC
    // Flip Y dans le shader (souvent nécessaire). Si chez toi c'est déjà bon, enlève le (1.0 - vUV.y).
    vec2 ndc = vec2(
        vUV.x * 2.0 - 1.0,
        ( vUV.y) * 2.0 - 1.0
    );

    // 2) Inverses calculées dans le shader
    mat4 invProj = inverse(cam.uProj);

    mat3 viewRot = mat3(cam.uView);
    mat3 invViewRot = transpose(viewRot);

    // 3) Ray en view space via invProj
    vec4 rayVS4 = invProj * vec4(ndc, 1.0, 1.0);
    vec3 dirVS  = normalize(rayVS4.xyz / rayVS4.w);

    // 4) View -> World (rotation only)
    vec3 dirWS = normalize(invViewRot * dirVS);


    //dirWS = -dirWS;
    dirWS.xz *= -1.0;
    dirWS.z *= -1.0;

    // 5) Mapping equirect (lat-long)
    float u = atan(dirWS.z, dirWS.x) / (2.0 * PI) + 0.5;
    float v = acos(clamp(dirWS.y, -1.0, 1.0)) / PI;

    out_color = texture(uSkyTex, vec2(u, v));
}
