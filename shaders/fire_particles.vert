#version 450

layout (location = 0) out flat uint VertexIndex;
layout (location = 1) out vec2 fragLocalPos; // [-1, 1] position locale dans le quad
layout (location = 2) out flat uint fragStep;

struct FireParticleStruct
{
    vec2 position; // Position of the fire particle
    uint step;     // Step count for the particle
};

layout (set = 0, binding = 0) readonly buffer Particles
{
    FireParticleStruct particles[32768]; // Array of fire particles
} particles;

const vec2 Vertex[6] = vec2[6](
    vec2(-1.0, -1.0),
    vec2( 1.0, -1.0),
    vec2(-1.0,  1.0),
    vec2( 1.0,  1.0),
    vec2(-1.0,  1.0),
    vec2( 1.0, -1.0)
);

void main()
{
    float size = 0.04; // Default size of the particle
    if (particles.particles[gl_InstanceIndex].step < 20)
    {
        size = 0.0080;
    }
    else if (particles.particles[gl_InstanceIndex].step < 100)
    {
        size = 0.0004 * float(particles.particles[gl_InstanceIndex].step); // Size increases with step count
    }
    else if (particles.particles[gl_InstanceIndex].step < 150)
    {
        size = 0.0005 * float(particles.particles[gl_InstanceIndex].step); // Size increases with step count
    }

    
    gl_Position = vec4(particles.particles[gl_InstanceIndex].position + Vertex[gl_VertexIndex] * size, 0.0, 1.0);
    

    VertexIndex = gl_VertexIndex;
    fragLocalPos = Vertex[gl_VertexIndex]; // pour connaître notre position locale dans le quad
    fragStep = particles.particles[gl_InstanceIndex].step;
}
