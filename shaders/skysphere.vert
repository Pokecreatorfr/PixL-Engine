#version 450

layout(location = 0) out vec2 vUV;

void main() {
    // Fullscreen triangle, 3 vertices: (0,0), (2,0), (0,2)
    vec2 pos = vec2((gl_VertexIndex << 1) & 2, (gl_VertexIndex & 2));
    vUV = pos * 0.5;                // (0..1)
    vec2 ndc = pos * 2.0 - 1.0;     // (-1..3) -> couvre tout l'écran
    gl_Position = vec4(ndc, 1.0, 1.0);
}
