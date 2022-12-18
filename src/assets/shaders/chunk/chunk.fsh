#version 330 core

layout(location = 0) out vec4 gPositionDepth;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec4 gAlbedoRoughness;

in vec3 Norm;
in vec2 TexCoord;
in vec3 FragPos;

flat in vec3 TriMtlId;
in vec3 TriMtlWeight;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D displacementMap;
uniform sampler2D roughnessMap;

// actual int. use for calculate Mtl UV in the Texture Atlas. {x=MtlId/MtlCap, y=0.0, w=1.0/MtlCap, h=1.0}
uniform float MtlCap;

uniform float debugVar1 = 0;
uniform float debugVar2 = 0;

const float MtlTexScale = 2.4;

int max_i(float a, float b, float c) {
    return a > b ? (a > c ? 0 : 2) : (b > c ? 1 : 2);
}


float inv_lerp(float t, float start, float end) {
    return (t - start) / (end - start);
}
const float P_NEAR = 0.01f;
const float P_FAR  = 1000.0f;
float linear_depth(float pprojdepth) {  // for perspective projection
    float z = pprojdepth * 2.0 - 1.0; // back to NDC
    float pDepth = (2.0 * P_NEAR * P_FAR) / (P_FAR + P_NEAR - z * (P_FAR - P_NEAR)); // [near, far]
    return inv_lerp(pDepth, P_NEAR, P_FAR); // [0,1] linear.
}

vec3 tri_samp(sampler2D tex, int MtlId, vec3 FragPos, vec3 blend) {

    float texScale = 1 / MtlTexScale;// 3.5;
    float ReginPosX  = (MtlId-1) / MtlCap;
    float ReginSizeX = 1.0 / MtlCap;
    vec2 uvX = vec2(mod(texScale * FragPos.z * ReginSizeX, ReginSizeX) + ReginPosX, texScale * FragPos.y);
    vec2 uvY = vec2(mod(texScale * FragPos.x * ReginSizeX, ReginSizeX) + ReginPosX, texScale * FragPos.z);
    vec2 uvZ = vec2(mod(texScale * FragPos.x * ReginSizeX, ReginSizeX) + ReginPosX, texScale * FragPos.y);

    return (
        texture(tex, uvX) * blend.x +
        texture(tex, uvY) * blend.y +
        texture(tex, uvZ) * blend.z
    ).rgb;
}

vec3 samp_norm(vec2 uv) {
    return (texture(normalMap, uv).rgb * 2.0 - 1.0);
}

void main()
{
    vec3 FragNorm = Norm;

    vec3 Albedo;

    float Roughness = 1.0;

    if (false) {  // no material id, use TexCoord.
        Albedo = texture(diffuseMap, TexCoord).rgb;
    } else {
        int mostWeightVert = max_i(TriMtlWeight.x, TriMtlWeight.y, TriMtlWeight.z);

        vec3 blend = pow(abs(Norm), vec3(6));  // more pow leads more [sharp at norm, mixing at tex]
        blend = blend / (blend.x + blend.y + blend.z);

        vec3 mtlw = pow(TriMtlWeight, vec3(0.6));  // 0.5-0.7. lesser -> more mix

        float h0 = tri_samp(displacementMap, int(TriMtlId[0]), FragPos, blend).r * mtlw[0];
        float h1 = tri_samp(displacementMap, int(TriMtlId[1]), FragPos, blend).r * mtlw[1];
        float h2 = tri_samp(displacementMap, int(TriMtlId[2]), FragPos, blend).r * mtlw[2];
        int mostHeightVert = max_i(h0, h1, h2);

        if (TriMtlId[mostHeightVert] == 0) {

            return;
        }

        const int MTL_STONE = 1,
                  MTL_GRASS = 2,
                  MTL_DIRT = 3;

        Albedo =
//        ((TriMtlId[0] == MTL_GRASS || TriMtlId[1] == MTL_GRASS || TriMtlId[2] == MTL_GRASS ) &&
//         (TriMtlId[0] == MTL_DIRT || TriMtlId[1] == MTL_DIRT || TriMtlId[2] == MTL_DIRT ) ) ?
//        tri_samp(diffuseMap, int(TriMtlId[0]), FragPos, blend) * TriMtlWeight[0] +
//        tri_samp(diffuseMap, int(TriMtlId[1]), FragPos, blend) * TriMtlWeight[1] +
//        tri_samp(diffuseMap, int(TriMtlId[2]), FragPos, blend) * TriMtlWeight[2]
//        :
        tri_samp(diffuseMap, int(TriMtlId[mostHeightVert]), FragPos, blend);

        Roughness = tri_samp(roughnessMap, int(TriMtlId[mostHeightVert]), FragPos, abs(Norm)).r;

        {
            int MtlId = int(TriMtlId[mostHeightVert]);
            float texScale = 1 / MtlTexScale;
            float ReginPosX  = (MtlId-1) / MtlCap;
            float ReginSizeX = 1.0 / MtlCap;
            vec2 uvX = vec2(mod(texScale * FragPos.z * ReginSizeX, ReginSizeX) + ReginPosX, texScale * FragPos.y);
            vec2 uvY = vec2(mod(texScale * FragPos.x * ReginSizeX, ReginSizeX) + ReginPosX, texScale * FragPos.z);
            vec2 uvZ = vec2(mod(texScale * FragPos.x * ReginSizeX, ReginSizeX) + ReginPosX, texScale * FragPos.y);

            vec3 tnormX = samp_norm(uvX);
            vec3 tnormY = samp_norm(uvY);
            vec3 tnormZ = samp_norm(uvZ);

            // GPU Gems 3, Triplanar Normal Mapping Method.
            FragNorm = normalize(
                vec3(0, tnormX.yx)          * blend.x +
                vec3(tnormY.x, 0, tnormY.y) * blend.y +
                vec3(tnormZ.xy, 0)          * blend.z +
                Norm
            );
        }

    }


    gPositionDepth = vec4(FragPos, linear_depth(gl_FragCoord.z));
    gNormal = FragNorm;
    gAlbedoRoughness = vec4(Albedo, Roughness);

}