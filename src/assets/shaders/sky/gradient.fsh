#version 330 core

out vec4 FragColor;

in vec3 FragPos;

uniform vec3 CameraPos;

const float fogNear = 10;
const float fogFar = 500;

void main() {

    // 3c8ecb
    vec3 mnColor = vec3(0.6*.9, 0.7*.9, 0.8*.9) * 0.7;
    mnColor = vec3(0.235, 0.557, 0.8);

    float t = clamp(smoothstep(fogNear, fogFar, length(CameraPos - FragPos)), 0.0, 1.0);
    FragColor = vec4(mnColor, 1.0-t);
}