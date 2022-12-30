#version 330 core

out vec4 FragColor;

in vec4 GeoColor;


void main() {

    FragColor = GeoColor;
}