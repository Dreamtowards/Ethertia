#version 330 core

out vec4 FragColor;

in vec3 Norm;
in vec2 TexCoord;

uniform sampler2D diffuseMap;

void main()
{
    vec3 lightDir = normalize(vec3(3, -9, 7));

    float f = max(0.4, dot(-lightDir, Norm));

    FragColor = vec4(0.8) * f * vec4(texture(diffuseMap, TexCoord).rgb, 1.0);
}