#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec2 TexCoord;

// 0,0: LeftTop. CCW to backward. 2 triangles.
const vec2 _tex[] = (
    (0, 0), (0, 1), (1, 1),
    (1, 1), (1, 0), (0, 0)
);

void main()
{
    TexCoord = _tex[gl_VertexIndex];

    gl_Position = vec4(TexCoord * 2.0 - 1.0, 0.0, 1.0);
}