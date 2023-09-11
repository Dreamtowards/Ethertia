#version 330 core
layout (location = 0) in vec3 in_pos;

out vec3 FragPos;

uniform mat4 matModel;
uniform mat4 matView;
uniform mat4 matProjection;

void main() {

    vec4 worldpos = matModel * vec4(in_pos, 1.0);
    gl_Position = matProjection * matView * worldpos;

    FragPos = worldpos.xyz;

}