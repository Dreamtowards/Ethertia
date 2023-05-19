#version 450
#extension GL_ARB_separate_shader_objects : enable

//#define OPT

layout(location = 0) in struct TE_Out
{
    vec3 WorldPos;
    vec2 TexCoord;
    vec3 WorldNorm;
    vec3 BaryCoord;
    vec3 MtlIds;
} fs_in;

layout(location = 0) out vec4 gPosition;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gAlbedo;

layout(set = 0, binding = 1) uniform UniformBufferFrag_T {
    int MtlTexCap;
    float MtlTexScale;
} ubo;

layout(set = 0, binding = 2) uniform sampler2D diffuseSampler;
layout(set = 0, binding = 3) uniform sampler2D normalSampler;
layout(set = 0, binding = 4) uniform sampler2D dramSampler;  // Disp, Refl, AO, Metal, +Emission? DRAM



int MaxValIdx(vec3 v) {
    float a=v.x;
    float b=v.y;
    float c=v.z;
    return a > b ? (a > c ? 0 : 2) : (b > c ? 1 : 2);
}

mat3 TriplanarUVs(vec3 FragWorldPos, int MtlTexId)
{
    float MtlCap = ubo.MtlTexCap;
    float MtlTexScale = ubo.MtlTexScale;
    vec3 p = FragWorldPos;
    float texScale = 1 / MtlTexScale;  // 3.5;
    float ReginPosX  = MtlTexId / MtlCap;
    float ReginSizeX = 1.0 / MtlCap;
    vec2 uvX = vec2(mod(texScale * p.z * ReginSizeX, ReginSizeX) + ReginPosX, texScale * p.y);
    vec2 uvY = vec2(mod(texScale * p.x * ReginSizeX, ReginSizeX) + ReginPosX, texScale * p.z);
    vec2 uvZ = vec2(mod(texScale * p.x * ReginSizeX, ReginSizeX) + ReginPosX, texScale * p.y);
    return mat3(
        uvX, 0,
        uvY, 0,
        uvZ, 0
    );
}

vec4 TriplanarSample(sampler2D tex, vec3 FragWorldPos, int MtlTexId, vec3 blend)
{
    mat3 uvXYZ = TriplanarUVs(FragWorldPos, MtlTexId);
    
#ifdef OPT
    return (
        texture(tex, uvXYZ[1].xy)
    ).rgba;
#else
    return (
        texture(tex, uvXYZ[0].xy) * blend.x +
        texture(tex, uvXYZ[1].xy) * blend.y +
        texture(tex, uvXYZ[2].xy) * blend.z
    ).rgba;
#endif
}

void main()
{
    vec3 Albedo = vec3(0);
    vec3 WorldPos   = fs_in.WorldPos;
    vec2 TexCoord   = fs_in.TexCoord;
    vec3 Norm       = fs_in.WorldNorm;
    vec3 BaryCoord  = fs_in.BaryCoord;
    vec3 MtlIds     = fs_in.MtlIds;

    int BaryIdx = MaxValIdx(BaryCoord.xyz);


    // when uv.y == 1000 (mtl magic number), means this vertex is a Pure MTL,
    // then use Triplanar Mapping etc to generate Albedo, Normal.
//    bool PureMTL = fract(MtlIds[BaryIdx]) == 0.5;
//    if (!PureMTL) {
//        // normal uv.
//        Albedo = texture(diffuseSampler, vec2(TexCoord.x, 1.0 - TexCoord.y)).rgb;
//    }
//    else
    {
        // PureMTL. use Triplanar Mapping.

        vec3 blend = pow(abs(Norm), vec3(6));  // more pow leads more [sharp at norm, mixing at tex]
             blend = blend / (blend.x + blend.y + blend.z);


        Albedo = TriplanarSample(diffuseSampler, WorldPos, int(MtlIds[BaryIdx]), blend).rgb;
    }

    // Gbuffer Output
    gPosition.xyz   = WorldPos;
    gPosition.w     = 1;
    gNormal.xyz = Norm;
    gNormal.w   = 1;
    gAlbedo.xyz = Albedo;  // BaryCoord
    gAlbedo.w   = 1;
}
