#version 330 core
out vec4 FragColor;

in vec3 TexCoord;

uniform samplerCube skyboxMap;

void main() {

    FragColor = texture(skyboxMap, TexCoord);
}
