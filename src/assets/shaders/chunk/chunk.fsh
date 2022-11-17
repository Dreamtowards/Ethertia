#version 330 core

out vec4 FragColor;

in vec3 Norm;
in vec2 TexCoord;
in vec3 FragPos;

flat in vec3 TriMtlId;
in vec3 TriMtlWeight;

uniform sampler2D diffuseMap;
uniform sampler2D displacementMap;
uniform vec3 CameraPos;

uniform float fogDensity;
uniform float fogGradient;

uniform vec3  cursorPos;
uniform float cursorSize;

// Material's Region in the TextureAtlas. xy: TexPos, zw: TexSize
uniform vec4 MaterialAtlasRegions[128];

uniform float debugRenderMode = 0;

int max_i(float a, float b, float c) {
    return a > b ? (a > c ? 0 : 2) : (b > c ? 1 : 2);
}

vec3 sampMtl(int MtlId, vec3 FragPos, vec3 Norm) {
    vec2 ReginPos  = MaterialAtlasRegions[MtlId].xy;
    vec2 ReginSize = MaterialAtlasRegions[MtlId].zw;
    return (
    texture(diffuseMap, vec2(mod(FragPos.x * ReginSize.x, ReginSize.x) + ReginPos.x, FragPos.z)) * abs(dot(vec3(0, 1, 0), Norm)) +
    texture(diffuseMap, vec2(mod(FragPos.z * ReginSize.x, ReginSize.x) + ReginPos.x, FragPos.y)) * abs(dot(vec3(1, 0, 0), Norm)) +
    texture(diffuseMap, vec2(mod(FragPos.x * ReginSize.x, ReginSize.x) + ReginPos.x, FragPos.y)) * abs(dot(vec3(0, 0, 1), Norm))
    ).rgb;
}

void main()
{
    vec3 lightDir = normalize(vec3(3, -9, 7));
    float lightf = max(0.4, dot(-lightDir, Norm));



    vec3 Albedo;

    if (false) {  // no material id, use TexCoord.
        Albedo = texture(diffuseMap, TexCoord).rgb;
    } else {
        int mxwTriIdx = max_i(TriMtlWeight.x, TriMtlWeight.y, TriMtlWeight.z);

        Albedo =
        debugRenderMode == 0 ?
        sampMtl(int(TriMtlId[0]), FragPos, Norm) * TriMtlWeight[0] +
        sampMtl(int(TriMtlId[1]), FragPos, Norm) * TriMtlWeight[1] +
        sampMtl(int(TriMtlId[2]), FragPos, Norm) * TriMtlWeight[2]
        :
        sampMtl(int(TriMtlId[mxwTriIdx]), FragPos, Norm);

        // texture(diffuseMap, vec2(mod(FragPos.x * ReginSize.x, ReginSize.x) + ReginPos.x, FragPos.z));
//            vec4(MtlId / 4);
//            (texture(diffuseMap, vec2(mod(FragPos.x * ReginSize.x, ReginSize.x) + ReginPos.x, FragPos.z)) * abs(dot(vec3(0, 1, 0), Norm)) +
//             texture(diffuseMap, vec2(mod(FragPos.z * ReginSize.x, ReginSize.x) + ReginPos.x, FragPos.y)) * abs(dot(vec3(1, 0, 0), Norm)) +
//             texture(diffuseMap, vec2(mod(FragPos.x * ReginSize.x, ReginSize.x) + ReginPos.x, FragPos.y)) * abs(dot(vec3(0, 0, 1), Norm)));
    }

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