#version 330 core
out vec4 FragColor;

in vec3 TexCoord;

uniform samplerCube skyboxMap;

uniform vec4 ColorMul;

void main() {


    FragColor = texture(skyboxMap, TexCoord) * ColorMul;
}
