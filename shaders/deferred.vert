#version 450
layout(location = 0) out vec2 v_uv;

void main()
{
    vec2 pos = vec2(
        (gl_VertexIndex == 1) ? 3.0 : -1.0,
        (gl_VertexIndex == 2) ? 3.0 : -1.0
    );
    
    v_uv = vec2(pos.x * 0.5 + 0.5, 1.0 - (pos.y * 0.5 + 0.5));
    gl_Position = vec4(pos, 0.0, 1.0);
}
