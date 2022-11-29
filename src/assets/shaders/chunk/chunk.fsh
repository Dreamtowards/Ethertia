#version 330 core

out vec4 FragColor;

in vec3 Norm;
in vec2 TexCoord;
in vec3 FragPos;

flat in vec3 TriMtlId;
in vec3 TriMtlWeight;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D displacementMap;
uniform vec3 CameraPos;

uniform float fogDensity;
uniform float fogGradient;

uniform vec3  cursorPos;
uniform float cursorSize;

// Material's Region in the TextureAtlas. xy: TexPos, zw: TexSize
uniform vec4 MaterialAtlasRegions[128];

uniform float debugRenderMode = 0;

const float MtlTexScale = 5;

int max_i(float a, float b, float c) {
    return a > b ? (a > c ? 0 : 2) : (b > c ? 1 : 2);
}

vec3 tri_samp(sampler2D tex, int MtlId, vec3 FragPos, vec3 blend) {

    float texScale = 1 / MtlTexScale;// 3.5;
    float ReginPosX  = MaterialAtlasRegions[MtlId].x;
    float ReginSizeX = MaterialAtlasRegions[MtlId].z;
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
    vec3 lightDir = normalize(vec3(2,-4,1));//3, -9, 7));

    vec3 FragNorm = Norm;

    vec3 Albedo;

    if (false) {  // no material id, use TexCoord.
        Albedo = texture(diffuseMap, TexCoord).rgb;
    } else {
        int mostWeightVert = max_i(TriMtlWeight.x, TriMtlWeight.y, TriMtlWeight.z);

        vec3 blend = abs(Norm);
        // vec3 blend = pow(abs(Norm), vec3(3));  // why pow?
        // blend = blend / (blend.x + blend.y + blend.z);

        vec3 mtlw = pow(TriMtlWeight, vec3(0.6));  // 0.5-0.7. lesser -> more mix

        float h0 = tri_samp(displacementMap, int(TriMtlId[0]), FragPos, blend).r * mtlw[0];
        float h1 = tri_samp(displacementMap, int(TriMtlId[1]), FragPos, blend).r * mtlw[1];
        float h2 = tri_samp(displacementMap, int(TriMtlId[2]), FragPos, blend).r * mtlw[2];
        int mostHeightVert = max_i(h0, h1, h2);


        Albedo =
//        debugRenderMode == 0 ?
//        tri_samp(diffuseMap, int(TriMtlId[0]), FragPos, blend) * TriMtlWeight[0] +
//        tri_samp(diffuseMap, int(TriMtlId[1]), FragPos, blend) * TriMtlWeight[1] +
//        tri_samp(diffuseMap, int(TriMtlId[2]), FragPos, blend) * TriMtlWeight[2];
//        :
        tri_samp(diffuseMap, int(TriMtlId[mostHeightVert]), FragPos, blend);

        {
            int MtlId = int(TriMtlId[mostHeightVert]);
            float texScale = 1 / MtlTexScale;
            float ReginPosX  = MaterialAtlasRegions[MtlId].x;
            float ReginSizeX = MaterialAtlasRegions[MtlId].z;
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
            FragNorm = pow(FragNorm, vec3(2));
        }

        // texture(diffuseMap, vec2(mod(FragPos.x * ReginSize.x, ReginSize.x) + ReginPos.x, FragPos.z));
//            vec4(MtlId / 4);
//            (texture(diffuseMap, vec2(mod(FragPos.x * ReginSize.x, ReginSize.x) + ReginPos.x, FragPos.z)) * abs(dot(vec3(0, 1, 0), Norm)) +
//             texture(diffuseMap, vec2(mod(FragPos.z * ReginSize.x, ReginSize.x) + ReginPos.x, FragPos.y)) * abs(dot(vec3(1, 0, 0), Norm)) +
//             texture(diffuseMap, vec2(mod(FragPos.x * ReginSize.x, ReginSize.x) + ReginPos.x, FragPos.y)) * abs(dot(vec3(0, 0, 1), Norm)));
    }


    float lightf = max(0.2, dot(-lightDir, FragNorm));

    FragColor = vec4(lightf * Albedo, 1.0);

    FragColor.r += min(0.5, max(0.0, cursorSize - length(cursorPos - FragPos)));


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