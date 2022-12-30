#version 330 core
layout(location = 0) in vec2 in_pos;
layout(location = 1) in vec2 in_texCoord;

out vec2 TexCoord;

void main() {

    gl_Position = vec4(in_pos.xy, 0.0, 1.0);

    TexCoord = in_texCoord;
}