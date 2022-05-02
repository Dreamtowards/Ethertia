#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform vec4 color;
uniform sampler2D tex;

uniform float border;
uniform float round;
uniform vec2 PixSize;

float sdbox(vec2 p, vec2 b) {
    vec2 q = abs(p)-b;
    return length(max(q, 0.0)) + min(max(q.x, q.y), 0);
}

void main() {

    FragColor = color * texture(tex, TexCoord);


    vec2 PixPos = (TexCoord * 2.0 - 1.0) * PixSize/2;
    float dist = sdbox(PixPos, PixSize/2 - round) - round;

    if (dist > 0 || dist < -border)
        discard;
}