#version 330 core

layout(location = 0) in vec3 in_pos;
layout(location = 2) in vec3 in_norm;

out vec3 VertNorm;

uniform mat4 matModel;
uniform mat4 matView;
uniform mat4 matProjection;

void main() {

    mat4 MVP = matProjection * matView * matModel;

    gl_Position = MVP * vec4(in_pos, 1.0);

    VertNorm = (vec3(MVP * vec4(in_norm, 0.0)));
}