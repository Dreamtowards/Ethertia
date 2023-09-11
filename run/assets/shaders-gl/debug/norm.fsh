#version 330 core

out vec4 FragColor;

in vec4 GeoColor;
in vec3 FragPos;


uniform vec3 limitPos;
uniform float limitLen;

void main() {


    if (limitLen != 0 && length(limitPos - FragPos) > limitLen)
        discard;

    FragColor = GeoColor;

}