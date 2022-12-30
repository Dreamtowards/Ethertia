#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D gPositionDepth;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoRoughness;

uniform sampler2D panoramaMap;

uniform vec3 CameraPos;
uniform mat4 matInvVP;

uniform float fogDensity;
uniform float fogGradient;

uniform vec3  cursorPos;
uniform float cursorSize;

uniform mat4 matInvView;
uniform mat4 matInvProjection;

uniform float debugVar0 = 0;
uniform float debugVar1 = 0;
uniform float debugVar2 = 0;

uniform vec3 tmpLightDir;
uniform vec3 tmpLightPos;

uniform float Time = 0;

// plane degined by p (p.xyz must be normalized)
float rayPlane( vec3 ro, vec3 rd, vec4 p )
{
    return -(dot(ro,p.xyz)+p.w)/dot(rd,p.xyz);
}
// axis aligned box centered at the origin, with size boxSize
vec2 rayBox( vec3 ro, vec3 rd, vec3 boxSize) //, vec3 outNormal )
{
    vec3 m = 1.0/rd; // can precompute if traversing a set of aligned boxes
    vec3 n = m*ro;   // can precompute if traversing a set of aligned boxes
    vec3 k = abs(m)*boxSize;
    vec3 t1 = -n - k;
    vec3 t2 = -n + k;
    float tN = max( max( t1.x, t1.y ), t1.z );
    float tF = min( min( t2.x, t2.y ), t2.z );
    if( tN>tF || tF<0.0) return vec2(-1.0); // no intersection
//    outNormal = (tN>0.0) ? step(vec3(tN),t1)) : // ro ouside the box
//    step(t2, vec3(tF)));  // ro inside the box
//    outNormal *= -sign(rd);
    return vec2( tN, tF );
}

// sphere of size ra centered at point ce
vec2 raySphere( vec3 ro, vec3 rd, vec3 ce, float ra )
{
    vec3 oc = ro - ce;
    float b = dot( oc, rd );
    float c = dot( oc, oc ) - ra*ra;
    float h = b*b - c;
    if( h<0.0 ) return vec2(-1.0); // no intersection
    h = sqrt( h );
    return vec2( -b-h, -b+h );
}
//vec4 rayCone( in vec3  ro, in vec3  rd, in vec3  pa, in vec3  pb, in float ra, in float rb )
//{
//    vec3  ba = pb - pa;
//    vec3  oa = ro - pa;
//    vec3  ob = ro - pb;
//    float m0 = dot(ba,ba);
//    float m1 = dot(oa,ba);
//    float m2 = dot(rd,ba);
//    float m3 = dot(rd,oa);
//    float m5 = dot(oa,oa);
//    float m9 = dot(ob,ba);
//
//    // caps
//    if( m1<0.0 )
//    {
//        if( dot2(oa*m2-rd*m1)<(ra*ra*m2*m2) ) // delayed division
//        return vec4(-m1/m2,-ba*inversesqrt(m0));
//    }
//    else if( m9>0.0 )
//    {
//        float t = -m9/m2;                     // NOT delayed division
//        if( dot2(ob+rd*t)<(rb*rb) )
//        return vec4(t,ba*inversesqrt(m0));
//    }
//
//    // body
//    float rr = ra - rb;
//    float hy = m0 + rr*rr;
//    float k2 = m0*m0    - m2*m2*hy;
//    float k1 = m0*m0*m3 - m1*m2*hy + m0*ra*(rr*m2*1.0        );
//    float k0 = m0*m0*m5 - m1*m1*hy + m0*ra*(rr*m1*2.0 - m0*ra);
//    float h = k1*k1 - k2*k0;
//    if( h<0.0 ) return vec4(-1.0); //no intersection
//    float t = (-k1-sqrt(h))/k2;
//    float y = m1 + t*m2;
//    if( y<0.0 || y>m0 ) return vec4(-1.0); //no intersection
//    return vec4(t, normalize(m0*(m0*(oa+t*rd)+rr*ba*ra)-ba*hy*y));
//}

vec2 samplePanoramaTex(vec3 rd) {
#define PI 3.14159265
    return vec2(atan(rd.z, rd.x) + PI, acos(-rd.y)) / vec2(2.0 * PI, PI);
}

vec3 compute_pixel_ray() {
    vec2 ndc = vec2(TexCoord * 2.0 - 1.0);
    vec4 rayClip = vec4(ndc.xy, -1.0, 1.0);

    vec4 rayView = matInvProjection * rayClip;
    rayView = vec4(rayView.xy, -1.0, 0.0);

    vec4 rayWorld = matInvView * rayView;
    return normalize(vec3(rayWorld));
}




//////////////// Atomsphere ////////////////

const vec3 SunPos = vec3(-1000, 1000, -1000);
const vec3 dirToSun = normalize(vec3(-1,1,-1));//normalize(SunPos - inScatterPoint);
const vec3 PlanetPos = vec3(0, 0, 0);
const float PlanetRadius = 20.0;
const float AtmosphereRaius = 500.0;
//const float densityFalloff = 1.0;


float DensityAtPoint(vec3 P) {  float densityFalloff = debugVar1;
    // t from gound 0.0, to atmosphere 1.0.
    float t = (length(P - PlanetPos) - PlanetRadius) / (AtmosphereRaius - PlanetRadius);
    float localDensity = exp(-t * densityFalloff) * (1.0 - t);
    return localDensity;
}

float DensityAlongRay(vec3 RayPos, vec3 RayDir, float RayLen) {  const int numOpticalDepthPoints = 10;
    vec3 P = RayPos;
    float step = RayLen / (numOpticalDepthPoints);
    float sumDensity = 0;

    for (int i = 0;i < numOpticalDepthPoints;++i) {
        sumDensity += DensityAtPoint(P) * step;
        P += RayDir * step;
    }
    return sumDensity;
}

vec3 ComputeLight(vec3 RayPos, vec3 RayDir, float RayLen, vec3 origCol) {  const int numInScatteringPoints = 10;
    vec3 inScatterPoint = RayPos;
    float step = RayLen / (numInScatteringPoints);  //!Why -1?
    vec3 inScatteredLight = vec3(0);

    vec3 wavelengths = vec3(680, 550, 440);  //440, 550, 680  //Seb: 700, 530, 440
    float scatteringStrength = 1;//1.0;
    vec3 scatterRGB = pow(400 / vec3(wavelengths), vec3(4)) * scatteringStrength;

    float last_viewRayOpticalDepth;

    for (int i = 0;i < numInScatteringPoints;++i) {
        float  sunRayLen = raySphere(inScatterPoint, dirToSun, PlanetPos, AtmosphereRaius).y;
        float  sunRayOpticalDepth = DensityAlongRay(inScatterPoint, dirToSun, sunRayLen);
        float viewRayOpticalDepth = DensityAlongRay(inScatterPoint, -RayDir, step * i); //!Why -RayDir
        vec3 transmittance = exp(-(sunRayOpticalDepth + viewRayOpticalDepth) * scatterRGB);
        float localDensity = DensityAtPoint(inScatterPoint);

        inScatteredLight += localDensity* transmittance * scatterRGB * step;
        inScatterPoint += RayDir * step;
        last_viewRayOpticalDepth = viewRayOpticalDepth;
    }

    // float origColTransmittance = exp(-last_viewRayOpticalDepth);
    return origCol * (1.0 - inScatteredLight)
    + inScatteredLight;
}





void main() {

    vec4 _PosDepth = texture(gPositionDepth, TexCoord).rgba;
    vec3 FragPos = _PosDepth.xyz;

    const float P_NEAR = 0.01f;
    const float P_FAR  = 1000.0f;
    float FragDepth = _PosDepth.w * (P_FAR - P_NEAR) + P_NEAR;

    vec4 _AlbRoug = texture(gAlbedoRoughness, TexCoord).rgba;
    vec3 Albedo  = _AlbRoug.rgb;
    float Roughness = _AlbRoug.a;

    vec3 Norm = texture(gNormal, TexCoord).rgb;

//    if (dot(Norm, CameraPos - FragPos) > 0)
//    Norm = - Norm;

    vec3 SunPos = vec3(-3, 2, -1) * 10000;
    vec3 SunColor = vec3(2.0);


    vec3 FragToSun = normalize(SunPos - FragPos);
    vec3 FragToCamera = normalize(CameraPos - FragPos);

    float diff = max(0.2, dot(FragToSun, Norm));
    vec3 diffColor = diff * SunColor;

    float specularIntensity = (1.0 - Roughness);
    float shininess = 48;
    vec3 lightReflect = reflect(-FragToSun, Norm);
    float spec = pow(max(dot(lightReflect, FragToCamera), 0.0), shininess);
    vec3 specColor = spec * specularIntensity * SunColor;

    FragColor = vec4((diffColor + specColor) * Albedo, 1.0);
//    FragColor = vec4(0,0,0,1);

    FragColor.rgb += 0.1 * min(1.0, max(0.0, cursorSize - length(cursorPos - FragPos)));



//    float viewLen = length(CameraPos - FragPos);
//    float fogf = clamp(pow(viewLen * fogDensity, fogGradient), 0.0, 1.0);
//    vec3 fogColor = vec3(0.5, 0.6, 0.8) * 0.8;
//    fogColor = vec3(0.235, 0.557, 0.8) * 0.9;
//    FragColor.rgb = mix(FragColor.rgb, fogColor, fogf);



    vec3 RayPos = CameraPos;
    vec3 RayDir = compute_pixel_ray();

    {
        vec3 LightPos = tmpLightPos;
        vec3 LighColor = vec3(1, 1.8, 1);

        vec3 LightDir = tmpLightDir;
        float LightAttenution = 100;

        float coneAngle = 0.8;
        float coneRound = 0;

        float distance = length(LightPos - FragPos);
        vec3 FragToLight = normalize(LightPos - FragPos);
        if (dot(-FragToLight, LightDir) > coneAngle) {
            FragColor.rgb += LighColor * (1.0 / (distance * 0.3));
        }

    }


//    vec2 hitPlanet = raySphere(RayPos, RayDir, PlanetPos, PlanetRadius);
//    if (hitPlanet.y - hitPlanet.x > 0.0) {
//        FragColor.b = (1);
//    }

//    vec2 hitAtmosphere = raySphere(RayPos, RayDir, PlanetPos, AtmosphereRaius);
//
//    float dstBeginAtmosphere = max(hitAtmosphere.x, 0);
//    float dstBetweenAtmosphere = min(max(hitAtmosphere.y, 0), FragDepth) - dstBeginAtmosphere;
////    float dstBetweenAtmosphere = min(hitAtmosphere.y, FragDepth - dstBeginAtmosphere);
////    min(hitAtmosphere.y, FragDepth) - dstBeginAtmosphere;
//
//    if (dstBetweenAtmosphere > 0.0) { float E = 0;//0.0001;
//        vec3 PointInAtmosphere = RayPos + RayDir * (dstBeginAtmosphere + E);
//        vec3 light = ComputeLight(PointInAtmosphere, RayDir, dstBetweenAtmosphere - E*2, FragColor.rgb);
//
//        FragColor.rgb = light;// FragColor.rgb * (1.0 - light) + (light);
//    }



//    vec3 cen = vec3(-20, 20, -20);


//    if (FragDepth > hitInfo.x)
//    FragColor.rgb +=  0.005 * vec3(min(hitInfo.y, FragDepth) - max(hitInfo.x, 0));// * (rayDir * 0.5f + 0.5f);

//    if (abs(texture(gPositionDepth, TexCoord - 0.002).a  - _PosDepth.a) > 0.0002)
//        FragColor = vec4(0, 0, 0, 1);

//    if (FragDepth > hitInfo.x)
//    FragColor.rgb += vec3((FragDepth / 100)) * (rayDir * 0.5f + 0.5f);
//    0.1 * (vec3(hitInfo.y - hitInfo.x)) * (rayDir * 0.5f + 0.5f);

//    if (specularIntensity > 0.5) {
//        vec3 camReflect = reflect(RayDir, Norm);
//        vec3 camRefract = refract(RayDir, Norm, 1.0 / 1.33);
//        FragColor.rgb = texture(panoramaMap, samplePanoramaTex(camRefract)).rgb;
//    }


    vec3 bgSkyColor = texture(panoramaMap, samplePanoramaTex(RayDir)).rgb;
    if (_PosDepth.w == 1.0f) {


        FragColor.rgb = bgSkyColor;
    } else {
            vec3 fogColor = vec3(0.5, 0.6, 0.8) * 1;
        fogColor = vec3(0.584, 0.58, 0.702);  // 9594b3

        FragColor.rgb = mix(FragColor.rgb, fogColor, FragDepth / 200.0f);
    }

}