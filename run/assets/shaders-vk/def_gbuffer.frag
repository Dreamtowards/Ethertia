#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 WorldPos;
layout(location = 1) in vec2 TexCoord;
layout(location = 2) in vec3 WorldNorm;

layout(location = 0) out vec4 gPosition;
//layout(location = 1) out vec3 gNormal;
//layout(location = 2) out vec3 gAlbedo;

layout(set = 0, binding = 1) uniform sampler2D diffuseSampler;
layout(set = 0, binding = 2) uniform sampler2D normalSampler;
layout(set = 0, binding = 3) uniform sampler2D dramSampler;  // Disp, Refl, AO, Metal

void main()
{

    // FragColor = vec4(TheColor * texture(texSampler, TexCoord).rgb, 1.0);

    gPosition = vec4(WorldPos, 0.5);
//    gNormal = WorldNorm;
//    gAlbedo = texture(diffuseSampler, TexCoord).rgb;
}
