#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D gPositionDepth;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoRoughness;
uniform sampler2D gAmbientOcclusion;
uniform sampler2D gShadowMap;

uniform sampler2D panoramaMap;

uniform vec3 CameraPos;
uniform mat4 matInvVP;

uniform float fogDensity;
uniform float fogGradient;

uniform vec3  cursorPos;
uniform float cursorSize;

uniform mat4 matView;
uniform mat4 matProjection;
uniform mat4 matInvView;
uniform mat4 matInvProjection;

uniform mat4 matShadowSpace;

uniform vec3 dbg_Color;

uniform float debugVar0 = 0;
uniform float debugVar1 = 0;
uniform float debugVar2 = 0;

uniform float DayTime;

uniform float Time = 0;

struct Light {
    vec3 position;
    vec3 color;
    vec3 attenuation;

    vec3 direction;
    vec2 coneAngle;  // xy: inner/outer cos
};
uniform Light lights[64];
uniform int lightsCount;



vec3 CalcPixelRay() {
    vec2 ndc = vec2(TexCoord * 2.0 - 1.0);
    vec4 rayClip = vec4(ndc.xy, -1.0, 1.0);

    vec4 rayView = matInvProjection * rayClip;
    rayView = vec4(rayView.xy, -1.0, 0.0);

    vec4 rayWorld = matInvView * rayView;
    return normalize(vec3(rayWorld));
}




float rayDisk( in vec3 ro, in vec3 rd, vec3 c, vec3 n, float r )
{
    vec3  o = ro - c;
    float t = -dot(n,o)/dot(rd,n);
    vec3  q = o + rd*t;
    return (dot(q,q)<r*r) ? t : -1.0;
}

float CalcShadow(vec3 FragPos)
{
    vec4 FragPosShadowSpace = matShadowSpace * vec4(FragPos, 1.0);

    vec3 proj = FragPosShadowSpace.xyz /= FragPosShadowSpace.w;  // perspective division. not necessary for Ortho Projection

    const float SHADOW_MAP_BIAS = 0.8;  // same of compose.fsh, shadow.fsh
    float distort_f = (1.0 - SHADOW_MAP_BIAS) + length(proj.xy) * SHADOW_MAP_BIAS;
    proj.xy /= distort_f;  // unwrap Dome Projection

    proj = proj * 0.5 + 0.5;  // to [0, 1]

    float lightingDepth = texture(gShadowMap, proj.xy).r;
    float fragDepth = proj.z;

    const float bias = 0.0005;
    float shadow = fragDepth - bias > lightingDepth ? 1.0 : 0.0;
    return shadow;
}


const float P_NEAR = 0.01f;
const float P_FAR  = 1000.0f;
float LinearDepth(float pdepth) {  // for perspective projection
    float f = P_NEAR * P_FAR / (P_FAR + pdepth * (P_NEAR - P_FAR)); // simplified. [near, far]
    return f / P_FAR;
}

//////// Screen Space Reflection ////////

vec2 ScreenPosOfWorldPos(vec3 worldp) {
    vec4 p = matProjection * matView * vec4(worldp, 1.0);
    p /= p.w;
    if(p.z < -1 || p.z > 1)
    return vec2(-1.0);
    p = p * 0.5f + 0.5f;
    return p.st;
}
float LinearDepthOfWorldPos(vec3 worldp) {
    vec4 p = matProjection * matView * vec4(worldp, 1.0);
    p /= p.w;
    p = p * 0.5f + 0.5f;
    return LinearDepth(p.z);
}
vec3 ReflectRayTracing(vec3 begin, vec3 dir, vec3 color) {
    const float stepLen = 0.025;
    vec3 p = begin;

    for (int i = 0; i < 24; ++i)
    {
        p += dir * pow(i+1, 2.86);
        vec2 uv = ScreenPosOfWorldPos(p);
        // if (uv.x < 0 || uv.y < 0 || uv.x > 1 || uv.y > 1) break;

        float sampleDepth = texture(gPositionDepth, uv).w;
        float testDepth = LinearDepthOfWorldPos(p);

        // && testDepth - sampleDepth < (testDepth * 200.0 + 1+i) / 2048.0
        if (sampleDepth < testDepth || i == 23) {
            return texture(gAlbedoRoughness, uv).rgb;
        }
    }
    return vec3(0);
}

void main() {

    vec4 _PosDepth = texture(gPositionDepth, TexCoord).rgba;
    vec3 FragPos = _PosDepth.xyz;

    float FragDepth = _PosDepth.w * P_FAR;

    vec4 _AlbRoug = texture(gAlbedoRoughness, TexCoord).rgba;
    vec3 Albedo  = _AlbRoug.rgb;
    float Roughness = _AlbRoug.a;

    vec3 Norm = texture(gNormal, TexCoord).rgb;



    vec3 SkyBg = vec3(0.529, 0.808, 0.957);
    vec3 SkyTop = vec3(0.298, 0.612, 0.831);

    SkyTop = dbg_Color;
//    SkyTop = vec3(0.16, 0.27, 0.45);   // Shader DarkBlue
//    SkyBg = vec3(0.51, 0.75, 0.83);
//    SkyTop = vec3(0.247, 0.394, 0.761);  // DeepBlueSky
//    SkyBg = vec3(0.471, 0.745, 0.933);
//    SkyTop = vec3(0.576, 0.569, 0.969); // MC Purple #9391f7
//    SkyBg = vec3(0.659, 0.718, 0.965);
//    SkyTop = vec3(0.31, 0.31, 0.41);  // Glow Sunset
//    SkyBg = vec3(0.90, 0.71, 0.36);
//    SkyTop = vec3(0.26, 0.29, 0.42);  // MC Sunset
//    SkyBg = vec3(0.49, 0.36, 0.36);
//    SkyTop = vec3(0.00, 0.00, 0.16);  // DeepSky
//    SkyBg = vec3(0.24, 0.72, 0.82);


    vec3 FragToCamera = normalize(CameraPos - FragPos);
    float specularIntensity = (1.0 - Roughness) * 0.3;
    float shininess = 128;

    vec3 sumDiffuse  = vec3(0);
    vec3 sumSpecular = vec3(0);
    for (int i = 0;i < lightsCount;i++) {
        Light light = lights[i];

        // Diffuse
        vec3 FragToLight = normalize(light.position - FragPos);
        vec3 Diffuse = light.color * Albedo * max(0.14, dot(FragToLight, Norm));

        // Specular
        // vec3 lightReflect = reflect(-FragToLight, Norm);
        // float spec = pow(max(dot(lightReflect, FragToCamera), 0.0), shininess);
        vec3 halfwayDir = normalize(FragToLight + FragToCamera);
        vec3 Specular = light.color * specularIntensity * pow(max(dot(halfwayDir, Norm), 0.0), shininess);

//        if (dot(-FragToLight, LightDir) > coneAngle) {
//            FragColor.rgb += LighColor * (1.0 / (distance * 0.3));
//        }

        float distance = length(light.position - FragPos);
        float Atten = 1.0 / (light.attenuation.x + light.attenuation.y * distance + light.attenuation.z * (distance*distance));

        sumDiffuse  += Diffuse  * Atten;
        sumSpecular += Specular * Atten;
    }

    float AO = texture(gAmbientOcclusion, TexCoord).r;
    float Ambient = 0.3 * AO;

    float Shadow = CalcShadow(FragPos);

    vec3 Lighting = (Ambient + max(0.24, (1.0 - Shadow)) *
                    (sumDiffuse + sumSpecular)) * Albedo;

    FragColor = vec4(Lighting, 1.0);

    if (Roughness < 0.35) {
        vec3 FragReflect = reflect(-FragToCamera, Norm);
        vec3 ReflectColor = ReflectRayTracing(FragPos, FragReflect, vec3(1));
        FragColor.rgb = ReflectColor;
    }


    // Brush hint.
    FragColor.rgb += 0.1 * max(0.0, (cursorSize - length(cursorPos - FragPos)) / (cursorSize*0.6) );

    FragColor.rgb += abs(cursorSize - length(cursorPos - FragPos)) < 0.004 ? 0.3 : 0;

//    {
//        vec3 dif = abs(cursorPos-FragPos);
////        float cross_f = min(dif.x, min(dif.y, dif.z)) < 0.01 ? 1 : 0;
////        float cross_f = abs(cursorPos.x - FragPos.x);
//        FragColor.rgb += min(dif.x, min(dif.z, dif.y)) < 0.005 ? 0.5 * max(0, cursorSize-length(cursorPos-FragPos)) : 0;
//    }

    // Fog
    float viewLen = length(CameraPos - FragPos);
    float fogf = clamp(pow(viewLen * fogDensity, fogGradient), 0.0, 1.0);
    vec3 fogColor = vec3(0.5, 0.6, 0.8) * 0.8;
    fogColor = vec3(0.235, 0.557, 0.8) * 0.9;
    FragColor.rgb = mix(FragColor.rgb, fogColor, fogf);



    vec3 RayPos = CameraPos;
    vec3 RayDir = CalcPixelRay();


//    FragColor.rgb = Norm;


    if (_PosDepth.w == 1.0f) {

//        float skydist = max(0, rayDisk(RayPos, RayDir, vec3(0,100, 0), vec3(0,1,0), 999999));
//        float skyTopColorFactor = skydist == 0 ? 0 : max(0, 1.0 - skydist / 800);
        float f = abs(dot(vec3(0,1,0), RayDir));

        FragColor.rgb = mix(SkyBg,SkyTop, min(1, f *2.5));
        FragColor.a = 0;//skyTopColorFactor;
//        discard;
        //FragColor.rgb = bgSkyColor;
    } else {
//        vec3 fogColor = SkyBg;//vec3(0.5, 0.6, 0.8);
////        fogColor = vec3(0.584, 0.58, 0.702);
//        fogColor *= 0.5f;  // * SunColor
//
//        FragColor.rgb = mix(FragColor.rgb, fogColor, min(1.0, FragDepth / 100.0f));
    }

}