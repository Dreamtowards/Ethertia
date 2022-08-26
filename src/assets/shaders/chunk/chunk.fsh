#version 330 core

out vec4 FragColor;

in vec3 Norm;
in vec2 TexCoord;
in vec3 FragPos;

uniform sampler2D diffuseMap;
uniform vec3 CameraPos;

uniform float fogDensity;
uniform float fogGradient;

void main()
{
    vec3 lightDir = normalize(vec3(3, -9, 7));
    float lightf = max(0.4, dot(-lightDir, Norm));

    FragColor = vec4(vec3(lightf), 1.0) * texture(diffuseMap, TexCoord);

//    if (FragColor.a != 1.0f)
//        discard;
//

//    float viewLen = length(CameraPos - FragPos);
//    float fogf = clamp(pow(viewLen * fogDensity, fogGradient), 0.0, 1.0);
//    vec3 fogColor = vec3(0.5, 0.6, 0.8) * 0.8;
//    fogColor = vec3(0.235, 0.557, 0.8) * 0.9;
//
//    FragColor.rgb = mix(FragColor.rgb, fogColor, fogf);

}