#version 330 core
layout(location = 0) in vec3 in_pos;

uniform mat4 matView;
uniform mat4 matProjection;

uniform vec3 direction;
uniform vec3 position;

void main() {

    gl_Position = matProjection * matView * vec4(position + in_pos * direction, 1.0);
}