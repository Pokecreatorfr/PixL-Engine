#version 450 core
layout(location=0) in vec2 vUV;
layout(location=1) in vec4 vColor;
layout(location=0) out vec4 oColor;
layout(binding=0) uniform sampler2D uTex;
void main(){ oColor = texture(uTex, vUV) * vColor; }

