#version 330 core

out vec4 FragColor;

in vec3 Norm;
in vec2 TexCoord;
in vec3 FragPos;

uniform sampler2D diffuseMap;
uniform vec3 CameraPos;

const float fogDensity = 0.02f;
const float fogGradient = 1.5f;

void main()
{
    vec3 lightDir = normalize(vec3(3, -9, 7));
    float lightf = max(0.4, dot(-lightDir, Norm));

    float viewLen = length(CameraPos - FragPos);

    FragColor = vec4(vec3(lightf) * texture(diffuseMap, TexCoord).rgb, 1.0);

    float fogf = clamp(pow(viewLen * fogDensity, fogGradient), 0.0, 1.0);
    vec3 fogColor = vec3(0.5, 0.6, 0.8) * 0.8;
    FragColor.rgb = mix(FragColor.rgb, fogColor, fogf);

//    FragColor.rgb = vec3(viewLen / 100);
}