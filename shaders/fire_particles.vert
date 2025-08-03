#version 450

layout (location = 0) out flat uint VertexIndex;
layout (location = 1) out flat uint fragStep;

struct FireParticleStruct
{
    vec2 position; // Position of the fire particle
    uint step;     // Step count for the particle
    float rotation; // Rotation of the particle in degrees
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
    else if (particles.particles[gl_InstanceIndex].step < 250)
    {
        size = 0.0004 * float(particles.particles[gl_InstanceIndex].step); // Size increases with step count
    }

    // Apply rotation to the vertex
    float rotationRad = radians(particles.particles[gl_InstanceIndex].rotation);
    float cosRot = cos(rotationRad);
    float sinRot = sin(rotationRad);
    
    vec2 rotatedVertex = vec2(
        Vertex[gl_VertexIndex].x * cosRot - Vertex[gl_VertexIndex].y * sinRot,
        Vertex[gl_VertexIndex].x * sinRot + Vertex[gl_VertexIndex].y * cosRot
    );
    
    gl_Position = vec4(particles.particles[gl_InstanceIndex].position + rotatedVertex * size, 0.0, 1.0);
    

    VertexIndex = gl_VertexIndex;
    fragStep = particles.particles[gl_InstanceIndex].step;
}
