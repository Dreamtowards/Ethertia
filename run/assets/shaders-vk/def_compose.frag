#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 TexCoord;

layout(location = 0) out vec4 FragColor;

layout(set = 0, binding = 0) uniform UBO_T {
    struct Light {
        vec3 position;
        vec3 color;
        vec3 attenuation;

        vec3 direction;
        vec2 coneAngle;  // xy: inner/outer cos
    } lights[64];
    int numLights;

    vec3 CameraPos;  // WorldSpace

    // mat4 matShadowSpace;
    // float DayTime;
} ubo;

layout(set = 0, binding = 1) uniform sampler2D gPosition;
layout(set = 0, binding = 2) uniform sampler2D gNormal;
layout(set = 0, binding = 3) uniform sampler2D gAlbdeo;
layout(set = 0, binding = 4) uniform sampler2D gDRAM;

void main()
{
    // Read Gbuffer
    vec4  _PosDep  = texture(gPosition, TexCoord);
    vec3  FragPos = _PosDep.xyz;
    float Depth    = _PosDep.w;

    vec3 FragNorm = texture(gNormal, TexCoord).xyz;
    vec3 Albedo = texture(gAlbdeo, TexCoord).xyz;

    vec4 _DRAM = texture(gDRAM, TexCoord);
    float DispHeight = _DRAM.x;  // DisplacementMap
    float Roughness = _DRAM.y;
    float AO = _DRAM.z;

    vec3 LightPos = vec3(0, 100, 0);
    vec3 LightColor = vec3(1, 1, 1);

    vec3 FragToLight = normalize(LightPos - FragPos);
    Albedo *= LightColor * max(0.2, dot(FragNorm, FragToLight));

    FragColor.rgb = Albedo;
    FragColor.a = 1;
}