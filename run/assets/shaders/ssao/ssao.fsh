#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D gPositionDepth;
uniform sampler2D gNormal;

uniform sampler2D texNoise;
uniform vec2 noiseScale;  // vec2(screenSize/noiseSize)

uniform vec3 kernelSamples[64];
const int kernelSize = 64;
const float radius = 1;

uniform mat4 matProjection;
uniform mat4 matView;


const float P_NEAR = 0.01f;
const float P_FAR  = 1000.0f;
float LinearDepth(float pprojdepth) {  // for perspective projection
    float z = pprojdepth * 2.0 - 1.0; // back to NDC
    float pDepth = (2.0 * P_NEAR * P_FAR) / (P_FAR + P_NEAR - z * (P_FAR - P_NEAR)); // [near, far]
    return pDepth / P_FAR; // [0,1] linear.
}

void main()
{
    vec3 FragPos  = texture(gPositionDepth, TexCoord).xyz;
    vec3 FragNorm = texture(gNormal,        TexCoord).xyz;
//    vec3 RandVec  = texture(texNoise,       TexCoord * noiseScale).xyz;

//    vec3 Tangent   = normalize(RandVec - FragNorm * dot(RandVec, FragNorm));
//    vec3 Bitangent = cross(FragNorm, Tangent);
//    mat3 TBN = mat3(Tangent, Bitangent, FragNorm);

    // float Dep = texture(gPositionDepth, TexCoord).w;

    float occlusion = 0.0;
    for (int i = 0;i < kernelSize; ++i) {
        vec3 dt = kernelSamples[i] * 0.001;
        if (abs(texture(gPositionDepth, TexCoord + dt.xy).w - texture(gPositionDepth, TexCoord).w) > 0.001) {
            occlusion += 1.0;
        }

//        vec3 samp = TBN * kernelSamples[i];
//        samp = FragPos + samp * radius;

//        vec4 offset = matProjection * matView * vec4(samp, 1.0);  // * matView
//        offset.xyz /= offset.w;
//        offset.xyz  = offset.xyz * 0.5 + 0.5;  // 0-1
//
//        float selfDepth = LinearDepth(offset.z);
//        float sampDepth = texture(gPositionDepth, offset.xy).w;
        // float rangeCheck = smoothstep(0.0, 1.0, radius / abs(FragPos.z - sampDepth));
//        occlusion += (sampDepth >= selfDepth ? 1.0 : 0.0);
    }
    occlusion = 1.0 - (occlusion / kernelSize);


//    float diff = texture(gPositionDepth, TexCoord).w - texture(gPositionDepth, TexCoord-vec2(0.001)).w;
    // vec3(abs(diff) > 0.0001 ? 1 : 0);
    FragColor.rgb = vec3(occlusion);
    FragColor.a = 1.0;
}