#version 330 core
layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec2 in_texCoord;
layout (location = 2) in vec3 in_norm;

uniform mat4 mvpModel;
uniform mat4 mvpView;
uniform mat4 mvpProj;

out vec3 Norm;
out vec2 TexCoord;

void main()
{
    vec4 worldpos = mvpModel * vec4(in_pos.xyz, 1.0);
    gl_Position = mvpProj * mvpView * worldpos;

    Norm = in_norm;

    TexCoord = in_texCoord;
}