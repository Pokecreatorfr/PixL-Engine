#version 450

layout (location = 0) in flat uint VertexIndex;
layout (location = 1) in vec2 fragLocalPos; // position dans le quad [-1, 1]
layout (location = 2) in flat uint fragStep;

layout (location = 0) out vec4 outColor;

void main()
{
    // Calcul distance au centre du quad
    float dist = length(fragLocalPos);

    // Masquage circulaire (cercle inscrit de rayon 1)
    if (dist > 1.0)
        discard;

    vec3 color;
    float alpha = 1.0; // Opacité par défaut

    if (fragStep < 50)
    {
        // jaune a rouge vif
        color = mix(vec3(1.0, 1.0, 0.0), vec3(1.0, 0.2, 0.0), fragStep / 50.0);
    }
    else if (fragStep < 150)
    {
        // rouge a noir 
        color = mix(vec3(1.0, 0.2, 0.0), vec3(0.1, 0.0, 0.0), fragStep / 100.0 - 0.5);
    }
    else
    {
        // noir a gris 
        color = mix(vec3(0.1, 0.0, 0.0), vec3(0.5, 0.5, 0.5), (fragStep - 150) / 100.0);
        alpha = mix(1.0, 0.3, (fragStep - 150) / 100.0); // alpha diminue
    }

    outColor = vec4(color, alpha);
}
