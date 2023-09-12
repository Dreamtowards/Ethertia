#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 TexCoord;

layout(location = 0) out vec4 FragColor;

struct Light {
    vec3 position;
    vec3 color;
    vec3 attenuation;

    vec3 direction;
    vec2 coneAngle;  // xy: inner/outer cos
};

layout(set = 0, binding = 0) uniform UBO_T {

    vec3 CameraPos;  // WorldSpace

    // for calculate RayDir of Ray View2Frag in WorldSpace. especially for invalid-gbuffer area. for sky render.
    mat4 invMatView;
    mat4 invMatProj;

    int lightsCount;
    Light lights[64];

    // mat4 matShadowSpace;
    // float DayTime;
} ubo;

layout(set = 0, binding = 1) uniform sampler2D gPosition;
layout(set = 0, binding = 2) uniform sampler2D gNormal;
layout(set = 0, binding = 3) uniform sampler2D gAlbdeo;
layout(set = 0, binding = 4) uniform sampler2D gDRAM;
//layout(set = 0, binding = 5) uniform samplerCube testCubeMap;



vec3 GetRayDir()
{
    vec2 uv = vec2(TexCoord.x, 1.0 - TexCoord.y);  // vulkan 2d LT coord, to world Y-up gl LB coord.
    vec2 ndc = vec2(uv * 2.0 - 1.0);
    vec4 rayClip = vec4(ndc.xy, -1.0, 1.0);

    vec4 rayView = ubo.invMatProj * rayClip;
    rayView = vec4(rayView.xy, -1.0, 0.0);

    vec4 rayWorld = ubo.invMatView * rayView;
    return normalize(vec3(rayWorld));
}


void main()
{
    // Read Gbuffer
    vec4  _PosDep  = texture(gPosition, TexCoord);
    vec3  WorldPos = _PosDep.xyz;
    float Depth    = _PosDep.w;

    vec3 CameraPos = ubo.CameraPos;
    vec3 RayDir = GetRayDir();

    if (Depth == 1.0) {
        float f = (dot(vec3(0,1,0), RayDir) + 1.0) / 2.0;
        vec3 SkyColor = vec3(0, 0.6, 1);
        FragColor = vec4(SkyColor * f, 1.0);

        //FragColor = texture(testCubeMap, RayDir);
        return;
    }

    vec3 WorldNorm = texture(gNormal, TexCoord).xyz;
    vec3 Albedo = texture(gAlbdeo, TexCoord).xyz;

    vec4 _DRAM = texture(gDRAM, TexCoord);
    float DispHeight = _DRAM.x;  // DisplacementMap
    float Roughness = _DRAM.y;
    float AO = _DRAM.z;

    vec3 FragToCamera = normalize(CameraPos - WorldPos);

    // Lighting

    vec3 totalDiffuse = vec3(0);
    vec3 totalSpecular = vec3(0);

    float specularIntensity = (1.0 - Roughness) * 0.3;
    float shininess = 1024;

    totalDiffuse += Albedo * 0.2;  // Ambient.

    for (int i = 0; i < ubo.lightsCount; ++i)
    {
        Light light = ubo.lights[i];

        // Diffuse
        vec3 FragToLight = normalize(light.position - WorldPos);
        vec3 Diffuse = Albedo * light.color * max(0.24, dot(FragToLight, WorldNorm));

        // Specular
        vec3 halfwayDir = normalize(FragToLight + FragToCamera);
        vec3 Specular = light.color * specularIntensity * pow(max(dot(halfwayDir, WorldPos), 0.0), shininess);

        // SpotLight
        //        if (dot(-FragToLight, LightDir) > coneAngle) {
        //            FragColor.rgb += LighColor * (1.0 / (distance * 0.3));
        //        }

        // Attenuation
        float distance = length(light.position - WorldPos);
        float Atten = dot(light.attenuation, light.attenuation) == 0 ? 1.0 : 
                      1.0 / (light.attenuation.x + light.attenuation.y * distance + light.attenuation.z * (distance*distance));

        totalDiffuse  += Diffuse  * Atten;
        totalSpecular += Specular * Atten;
    }



    FragColor.rgb = (totalDiffuse + totalSpecular);
    FragColor.a = 1;
}