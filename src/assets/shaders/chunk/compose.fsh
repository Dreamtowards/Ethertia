#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D gPositionDepth;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoRoughness;

uniform vec3 CameraPos;

uniform float fogDensity;
uniform float fogGradient;

uniform vec3  cursorPos;
uniform float cursorSize;

uniform float debugVar1 = 0;
uniform float debugVar2 = 0;

void main() {

    vec4 _PosDepth = texture(gPositionDepth, TexCoord).rgba;
    vec3 FragPos = _PosDepth.xyz;
    float FragDepth = _PosDepth.w;

    vec4 _AlbRoug = texture(gAlbedoRoughness, TexCoord).rgba;
    vec3 Albedo  = _AlbRoug.rgb;
    float Roughness = _AlbRoug.a;

    vec3 Norm = texture(gNormal, TexCoord).rgb;


    vec3 LightPos = vec3(-50, 50, -50);
    vec3 LightColor = vec3(1.0);


    vec3 FragToLight = normalize(LightPos - FragPos);
    vec3 FragToCamera = normalize(CameraPos - FragPos);

    vec3 ambient = 0.1 * LightColor;

    float diff = max(0.0, dot(FragToLight, Norm));
    vec3 diffColor = diff * LightColor;

    float specularIntensity = (1.0 - Roughness);
    float shininess = 48;
    vec3 lightReflect = reflect(-FragToLight, Norm);
    float spec = pow(max(dot(lightReflect, FragToCamera), 0.0), shininess);
    vec3 specColor = spec * specularIntensity * LightColor;

    FragColor = vec4((ambient + diffColor + specColor) * Albedo, 1.0);

    FragColor.rgb += 0.1 * min(1.0, max(0.0, cursorSize - length(cursorPos - FragPos)));


    if (FragDepth == 1.0f)
        discard;


    float viewLen = length(CameraPos - FragPos);
    float fogf = clamp(pow(viewLen * fogDensity, fogGradient), 0.0, 1.0);
    vec3 fogColor = vec3(0.5, 0.6, 0.8) * 0.8;
//    fogColor = vec3(0.235, 0.557, 0.8) * 0.9;

    FragColor.rgb = mix(FragColor.rgb, fogColor, fogf);

}