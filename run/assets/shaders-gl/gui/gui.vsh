#version 330 core
layout (location = 0) in vec2 in_pos;
layout (location = 1) in vec2 in_texCoord;

out vec2 TexCoord;

uniform vec4 vertPosSize;

uniform vec4 texPosSize;

void main() {

    gl_Position = vec4(vertPosSize.xy + in_pos.xy * vertPosSize.zw, 0.0, 1.0);

    TexCoord = texPosSize.xy + in_texCoord * texPosSize.zw;
}