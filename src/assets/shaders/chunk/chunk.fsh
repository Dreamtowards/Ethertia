#version 330 core

out vec4 FragColor;

in vec3 Norm;
in vec2 TexCoord;
in vec3 FragPos;

flat in vec3 TriMtlId;
in vec3 TriMtlWeight;

uniform sampler2D diffuseMap;
uniform vec3 CameraPos;

uniform float fogDensity;
uniform float fogGradient;

uniform vec3  cursorPos;
uniform float cursorSize;

// Material's Region in the TextureAtlas. xy: TexPos, zw: TexSize
uniform vec4 MaterialAtlasRegions[128];

//float inRange(float val, float begin, float size) {
//    return mod(val - begin, size) + begin;
//}
int max_i(float a, float b, float c) {
    return a > b ? (a > c ? 0 : 2) : (b > c ? 1 : 2);
}

void main()
{
    vec3 lightDir = normalize(vec3(3, -9, 7));
    float lightf = max(0.4, dot(-lightDir, Norm));





    vec4 Albedo;

    if (false) {  // no material id, use TexCoord.
        Albedo = texture(diffuseMap, TexCoord);
    } else {
        int triVertIdx = max_i(TriMtlWeight.x, TriMtlWeight.y, TriMtlWeight.z);
        int MtlId = int(TriMtlId[triVertIdx]);

        vec2 ReginPos =  MaterialAtlasRegions[int(MtlId)].xy;
        vec2 ReginSize = MaterialAtlasRegions[int(MtlId)].zw;

        Albedo = //texture(diffuseMap, vec2(mod(FragPos.x * ReginSize.x, ReginSize.x) + ReginPos.x, FragPos.z));
//            vec4(MtlId / 4);

            (texture(diffuseMap, vec2(mod(FragPos.x * ReginSize.x, ReginSize.x) + ReginPos.x, FragPos.z)) * abs(dot(vec3(0, 1, 0), Norm)) +
             texture(diffuseMap, vec2(mod(FragPos.z * ReginSize.x, ReginSize.x) + ReginPos.x, FragPos.y)) * abs(dot(vec3(1, 0, 0), Norm)) +
             texture(diffuseMap, vec2(mod(FragPos.x * ReginSize.x, ReginSize.x) + ReginPos.x, FragPos.y)) * abs(dot(vec3(0, 0, 1), Norm)));
    }

    FragColor = vec4(vec3(lightf) * Albedo.rgb, 1.0);

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