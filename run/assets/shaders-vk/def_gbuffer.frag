#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in struct VS_Out
{
    vec3 WorldPos;
    vec2 TexCoord;
    vec3 WorldNorm;
    vec3 BaryCoord;
    float MtlId;
} fs_in;

layout(location = 0) out vec4 gPosition;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gAlbedo;

layout(set = 0, binding = 1) uniform sampler2D diffuseSampler;
layout(set = 0, binding = 2) uniform sampler2D normalSampler;
layout(set = 0, binding = 3) uniform sampler2D dramSampler;  // Disp, Refl, AO, Metal

void main()
{
    vec3 Albedo = vec3(0);
    vec3 WorldPos = fs_in.WorldPos;
    vec2 TexCoord = fs_in.TexCoord;
    vec3 Norm = fs_in.WorldNorm;
    vec3 BaryCoord = fs_in.BaryCoord;


    // when uv.y == 1000 (mtl magic number), means this vertex is a Pure MTL,
    // then use Triplanar Mapping etc to generate Albedo, Normal.
//    bool PureMTL = fract(MtlId) == 0.5;
//    if (!PureMTL) {
//        // normal uv.
//        Albedo = texture(diffuseSampler, vec2(TexCoord.x, 1.0 - TexCoord.y)).rgb;
//    }
//    else
    {
        // PureMTL. use Triplanar Mapping.


        Albedo = texture(diffuseSampler, WorldPos.xz).rgb;
    }

    // Gbuffer Output
    gPosition.xyz = WorldPos;
    gPosition.w = 0.9;
    gNormal.xyz = Norm;
    gNormal.w = 0.9;
    gAlbedo.xyz = Albedo = BaryCoord;  // BaryCoord
    gAlbedo.w = 0.9;
}
