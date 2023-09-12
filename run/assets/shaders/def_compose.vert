#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec2 TexCoord;

// 0,0: LeftTop. CCW, Z-Back. 2 triangles.

const vec2 _uv[6] = {
    vec2(0, 0), vec2(0, 1), vec2(1, 1),
    vec2(1, 1), vec2(1, 0), vec2(0, 0)
};
const vec2 _pos[6] = {
    vec2(-1, 1), vec2(-1, -1), vec2(1, -1),
    vec2(1, -1), vec2(1, 1), vec2(-1, 1)
};

void main()
{
    TexCoord = _uv[gl_VertexIndex];

    gl_Position = vec4(_pos[gl_VertexIndex], 0.0, 1.0);
}