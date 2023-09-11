#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable

#include "util.h"

layout(location = 0) in struct VS_Out
{
    vec3 WorldPos;
    vec3 WorldNorm;
    vec3 BaryCoord;
    vec3 MtlIds;
} fs_in;

layout(location = 0) out vec4 gPosition;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gAlbedo;

layout(set = 0, binding = 1) uniform UniformBufferFrag_T {
    int MtlTexCap;
    float MtlTexScale;  // def 3.5
    float MtlTriplanarBlendPow;  // def 6.0
    float MtlHeightmapBlendPow;  // def 0.6
} ubo;

layout(set = 0, binding = 2) uniform sampler2D texDiffuse;
layout(set = 0, binding = 3) uniform sampler2D texNormal;
layout(set = 0, binding = 4) uniform sampler2D texDRAM;  // Disp, Refl, AO, Metal, +Emission? DRAM


///////// Common Util Func /////////

int MaxIdx(vec3 v) {
    float a=v.x;
    float b=v.y;
    float c=v.z;
    return a > b ? (a > c ? 0 : 2) : (b > c ? 1 : 2);
}

float LinearDepth(float perspDepth, float pNear, float pFar) {  // for perspective projection
    float f = pNear * pFar / (pFar + perspDepth * (pNear - pFar)); // simplified. [near, far]
    return f / pFar;  // [0, 1]
}



///////// Triplanar Mapping /////////

mat3 TriplanarUVs(vec3 FragWorldPos, int MtlTexId)
{
    float MtlCap = ubo.MtlTexCap;
    float MtlTexScale = ubo.MtlTexScale;
    vec3 p = FragWorldPos;
    float texScale = 1 / MtlTexScale;
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
    vec3 WorldPos    = fs_in.WorldPos;
    vec3 WorldNorm   = fs_in.WorldNorm;
    vec3 BaryCoord  = fs_in.BaryCoord;
    vec3 MtlIds     = fs_in.MtlIds;

    int MaxBary_i = MaxIdx(BaryCoord.xyz);  // aka HighestWeightVertIdx

//    // when uv.y == 1000 (mtl magic number), means this vertex is a Pure MTL,
//    // then use Triplanar Mapping etc to generate Albedo, Normal.
//    bool PureMTL = fract(MtlIds[HighestBaryIdx]) == 0.5;
//    if (!PureMTL) {
//        // normal uv.
//        vec4 diff = texture(diffuseSampler, vec2(TexCoord.x, 1.0 - TexCoord.y)).rgba;
//        Albedo = diff.rgb;
//
//        if (diff.a < 0.1) {
//            discard;
//        }
//    }
//    else
//    {
//        // PureMTL. use Triplanar Mapping.
//
//        vec3 blend = pow(abs(FragNorm), vec3(ubo.MtlTriplanarBlendPow));  // more pow leads more [sharp at norm, mixing at tex]
//             blend = blend / (blend.x + blend.y + blend.z);
//
//        int FragMtlId
//        = int(MtlIds[HighestBaryIdx]);
//
//#ifndef OPT
//        // HeightMap Transition.
//        vec3 heightMapBlend = pow(BaryCoord, vec3(ubo.MtlHeightmapBlendPow));  // 0.5-0.7. lesser -> more mix
//        float h0 = TriplanarSample(dramSampler, FragPos, int(MtlIds[0]), blend).r * heightMapBlend[0];
//        float h1 = TriplanarSample(dramSampler, FragPos, int(MtlIds[1]), blend).r * heightMapBlend[1];
//        float h2 = TriplanarSample(dramSampler, FragPos, int(MtlIds[2]), blend).r * heightMapBlend[2];
//        int HighestDispIdx = MaxValIdx(vec3(h0, h1, h2));  // mostHeightIdx
//        FragMtlId = int(MtlIds[HighestDispIdx]);
//#endif
//
////        if (FragMtlId == 0) {
////            discard;
////        }
//
//        Albedo = TriplanarSample(diffuseSampler, FragPos, FragMtlId, blend).rgb;
//
//        vec4 DRAM = TriplanarSample(dramSampler, FragPos, FragMtlId, blend).rgba;
//
//
//
//#define UnwrapNorm(uv) (texture(normalSampler, uv).rgb * 2.0 - 1.0)
//
//        mat3 uvXYZ = TriplanarUVs(FragPos, FragMtlId);
//        vec3 tnormX = UnwrapNorm(uvXYZ[0].xy);  // original texture space normal
//        vec3 tnormY = UnwrapNorm(uvXYZ[1].xy);
//        vec3 tnormZ = UnwrapNorm(uvXYZ[2].xy);
//
//        // GPU Gems 3, Triplanar Normal Mapping Method.
//        FragNorm = normalize(
//            vec3(0, tnormX.yx)          * blend.x +
//            vec3(tnormY.x, 0, tnormY.y) * blend.y +
//            vec3(tnormZ.xy, 0)          * blend.z +
//            FragNorm
//        );
//
//    }

    // Gbuffer Output
    gPosition.xyz   = WorldPos;
    //gPosition.w     = LinearDepth(gl_FragCoord.z, 0.01, 1000.0);
    //gNormal.xyz = FragNorm;
    //gNormal.w   = 1;
    //gAlbedo.xyz = Albedo;  // BaryCoord
    //gAlbedo.w   = 1;
}
