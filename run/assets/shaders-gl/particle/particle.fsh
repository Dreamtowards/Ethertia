#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D tex;

uniform vec2 uv_pos;
uniform vec2 uv_size;

void main()
{


    FragColor = texture(tex, TexCoord * uv_size + uv_pos);
}