#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D gPositionDepth;
uniform sampler2D gNormal;

uniform sampler2D texNoise;
uniform vec2 noiseScale;  // vec2(screenSize/noiseSize)

const int kernelSize = 32;
uniform vec3 kernelSamples[64];
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
    vec4 _FragPosDepth = texture(gPositionDepth, TexCoord).xyzw;
    float FragDepth = _FragPosDepth.w;

    if (FragDepth == 1.0) {
        FragColor = vec4(1.0);
        return;
    }

    vec3 FragNorm = mat3(matView) * texture(gNormal, TexCoord).xyz;

    // TBN for Random Rotate
    vec3 RandVec   = vec3(1,0,0);//texture(texNoise, TexCoord * noiseScale).xyz;
    vec3 Tangent   = normalize(RandVec - FragNorm * dot(RandVec, FragNorm));
    vec3 Bitangent = cross(FragNorm, Tangent);
    mat3 TBN = mat3(Tangent, Bitangent, FragNorm);

    vec3 ViewSpaceFragPos = (matView * vec4(_FragPosDepth.xyz, 1.0)).xyz;

    float occlusion = 0.0;
    for (int i = 0;i < kernelSize; ++i) {
        // View Space.
        vec3 samp = ViewSpaceFragPos + (TBN * kernelSamples[i]) * radius;
        float sampPointDepth = -samp.z;

        // Clip Space. offset to get screen position of sample point
        vec4 offset = matProjection * vec4(samp, 1.0);
        offset.xyz /= offset.w;
        offset.xyz  = offset.xyz * 0.5 + 0.5;  // 0-1
        float sampActualDepth = texture(gPositionDepth, offset.xy).w * P_FAR;

         float range_f = smoothstep(0.0, 1.0, radius / abs(-ViewSpaceFragPos.z - sampActualDepth));
        occlusion += (sampActualDepth <= sampPointDepth ? 1.0 : 0.0) * range_f;
    }
    float aof = 1.0 - (occlusion / kernelSize);


    FragColor.rgb = vec3(aof);
    FragColor.a = 1.0;
}