#version 450

layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec2 in_uv;  // TexId
layout(location = 2) in vec3 in_norm;

uniform mat4 matModel;
uniform mat4 matView;
uniform mat4 matProjection;



void main()
{
    vec4 worldpos = matModel * vec4(in_pos, 1.0);

    gl_Position = matProjection * matView * worldpos;
}