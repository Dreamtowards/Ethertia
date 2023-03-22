#version 330 core

layout(location = 0) in vec3 in_pos;
layout(location = 2) in vec3 in_norm;

out vec3 VertNorm;
out vec3 VertPos;

uniform mat4 matModel;
uniform mat4 matView;
uniform mat4 matProjection;

void main() {

    vec4 worldpos = matModel * vec4(in_pos, 1.0);

    VertPos = worldpos.xyz;

    gl_Position = matProjection * matView * worldpos;

    VertNorm = vec3(matProjection * matView * matModel * vec4(in_norm, 0.0));
}