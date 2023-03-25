#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform vec4 color;
uniform sampler2D tex;

uniform float border;
uniform float round;
uniform vec2 sizePixel;

uniform int channelMode = 0;  // 0: vec4(rgba), 1: vec4(rgb, 1.0), 2: vec4(aaa, 1.0).  for display GBuffer PositionDepth Buffer.

float sdbox(vec2 p, vec2 b) {
    vec2 q = abs(p)-b;
    return length(max(q, 0.0)) + min(max(q.x, q.y), 0);
}

void main() {

    FragColor = color * texture(tex, TexCoord);


    if (border < 99999 || round != 0) {
        vec2 posPixel = (TexCoord * 2.0 - 1.0) * sizePixel/2;
        float dist = sdbox(posPixel, sizePixel/2 - round) - round;

        if (dist > 0 || dist < -border)
        discard;
    }

         if (channelMode == 1) FragColor.a = 1.0;
    else if (channelMode == 2) FragColor = vec4(FragColor.aaa, 1.0);
}