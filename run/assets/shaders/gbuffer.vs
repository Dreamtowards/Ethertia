#version 450

layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec2 in_uv;  // TexId
layout(location = 2) in vec3 in_norm;

uniform mat4 matModel;
uniform mat4 matView;
uniform mat4 matProjection;

vec2 positions[3] = vec2[](
    vec2(0.0, -0.5),
    vec2(0.5, 0.5),
    vec2(-0.5, 0.5)
);

vec3 colors[3] = vec3[](
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
);

void main()
{
    //gl_Position = vec4(positions[gl_VertexID], 0.0, 1.0);


    vec4 worldpos = matModel * vec4(in_pos, 1.0);

    gl_Position = matProjection * matView * worldpos;
    //gl_Position.y = -gl_Position.y;
}