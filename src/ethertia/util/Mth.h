//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_MTH_H
#define ETHERTIA_MTH_H

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <cmath>
#include <btBulletCollisionCommon.h>

#include <ethertia/util/UnifiedTypes.h>

class Mth
{
public:
    static constexpr float PI = 3.14159265359f;
    static constexpr float PI_2 = 1.57079632679f;

    static constexpr float NaN = NAN;
    static constexpr float Inf = INFINITY;

    static inline bool isnan(float f) {
        return std::isnan(f);
    }


    static inline int floor(float v) {
        int i = (int)v;
        return (float)i>v ? i-1 : i;
    }
    static inline int floor(float v, int u) {
        int i = (int)floor(v / (float)u);
        return i*u;
    }
    static inline int ceil(float v) {
        return floor(v)+1;
    }
    static inline float floor(float v, float u) {
        return floor(v / u) * u;
    }

    /// u: unit
    static float round(float v, float u) {
        float f = std::round(v / u);
        return f*u;
    }
    static glm::vec3 round(glm::vec3 v, float u) {
        return glm::vec3(round(v.x, u),
                         round(v.y, u),
                         round(v.z, u));
    }

    static inline glm::vec3 floor(glm::vec3 v) {
        return floor(v, 1);
    }
    static inline glm::vec3 floor(glm::vec3 v, int u) {
        return glm::vec3(floor(v.x, u), floor(v.y, u), floor(v.z, u));
    }

    template<typename T>
    static inline T max(T a, T b) {
        return a > b ? a : b;
    }
    template<typename T>
    static inline T min(T a, T b) {
        return a < b ? a : b;
    }

    template<typename T>
    static inline T abs(T a) {
        return a < 0 ? -a : a;
    }

    template<typename T>
    static inline T sq(T a) {
        return a * a;
    }

    static inline float mod(float v, float u) {
        return std::fmod(v, u);
    }

    template<typename T>
    static inline T fade(T t) {
        return t*t*t*(t*(t*6-15)+10);
    }
    template<typename T>
    static inline T lerp(float t, T a, T b) {
        return a + t * (b - a);
    }
    template<typename T>
    static inline T clamp(T f, T a, T b) {
        return min(max(f, a), b);
    }

    static inline float rlerp(float v, float a, float b) {
        return (v - a) / (b - a);
    }

    static f32 hash(u32 i) {
        i = (i << 13) ^ i;
        return (((i * i * 15731 + 789221) * i + 1376312589) & 0xffffffff) / (float)0xffffffff;
    }

    static glm::vec3 angleh(float angle) {
        return glm::rotate(glm::mat4(1), angle, glm::vec3(0, 1, 0)) * glm::vec4(0, 0, -1, 1.0);
    }

    static btTransform btTransf(const glm::vec3& p, const btQuaternion& rot = btQuaternion::getIdentity()) {
        return btTransform(rot, btVector3(p.x, p.y, p.z));
    }
    static btVector3 btVec3(const glm::vec3& p) {
        return btVector3(p.x, p.y, p.z);
    }


    static float pow(float base, float exp) {
        return std::pow(base, exp);
    }

    static float signal(float v) {
        return v < 0 ? -1.0f :
              (v > 0 ?  1.0f : 0.0f);
    }

    static float frac(float v) {
        return v - Mth::floor(v);
    }

    // to radians
    static float radians(float deg) {
        return deg * 0.01745329251f;  // * 1/180.0 * PI
    }
    // to degrees
    static float degrees(float rad) {
        return rad * 57.2957795131f;// 1/PI * 180.0
    }

    static inline glm::vec3 vec3(const float* base) {
        return glm::vec3(base[0], base[1], base[2]);
    }
    static inline void vec3out(const glm::vec3& v, float* base) {
        base[0] = v.x;
        base[1] = v.y;
        base[2] = v.z;
    }

    static constexpr glm::vec3 QFACES[6] = {
            glm::vec3(-1, 0, 0),
            glm::vec3( 1, 0, 0),
            glm::vec3( 0,-1, 0),
            glm::vec3( 0, 1, 0),
            glm::vec3( 0, 0,-1),
            glm::vec3( 0, 0, 1),
    };

    static glm::mat4 matEulerAngles(glm::vec3 eulerAngles) {
        glm::mat4 rot = glm::mat4(1);

        if (eulerAngles.y != 0) rot = glm::rotate(rot, eulerAngles.y, glm::vec3(0, 1, 0));
        if (eulerAngles.x != 0) rot = glm::rotate(rot, eulerAngles.x, glm::vec3(1, 0, 0));
        if (eulerAngles.z != 0) rot = glm::rotate(rot, eulerAngles.z, glm::vec3(0, 0, 1));

        return rot;
    }

    // RH, Z-backward
    static glm::vec3 worldRay(float x, float y, float w, float h, const glm::mat4& proj, const glm::mat4& view) {
        using glm::vec2, glm::vec4;
        vec2 ndc = Mth::ndc(x, y, w, h);

        vec4 ray_clip = vec4(ndc.x, ndc.y, -1.0f, 1.0f);

        vec4 ray_view = glm::inverse(proj) * ray_clip;
        ray_view = vec4(ray_view.x, ray_view.y, -1.0f, 0.0f);

        vec4 ray_world = glm::inverse(view) * ray_view;

        return glm::normalize(glm::vec3(ray_world));
    }

    static glm::mat4 viewMatrix(glm::vec3 position, glm::vec3 eulerAngles) {
        glm::mat4 rot = glm::mat4(1);

        rot = glm::translate(rot, position);

        rot = rot * matEulerAngles(eulerAngles);

        return glm::inverse(rot);
    }

    static glm::vec3 eulerDirection(float yaw, float pitch) {
        float f0 = std::cos(-yaw - Mth::PI);
        float f1 = std::sin(-yaw - Mth::PI);
        float f2 = std::cos(-pitch);
        float f3 = std::sin(-pitch);
        return glm::normalize(glm::vec3(f1 * f2, f3, f0 * f2));
    }

    static glm::vec3 projectWorldpoint(const glm::vec3& worldpos, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
        glm::vec4 p = projectionMatrix * viewMatrix * glm::vec4(worldpos, 1.0f);
        p.x /= p.w;
        p.y /= p.w;

        p.x =        (p.x + 1.0f) / 2.0f;
        p.y = 1.0f - (p.y + 1.0f) / 2.0f;

        return glm::vec3(p);
    }

//    static glm::mat4 calculateViewMatrix(glm::vec3 position, glm::mat3 rotation) {
//        glm::mat4 view = glm::mat4(1);// = glm::mat4(rotation);
//
//        view = glm::translate(view, position);
//
//        return glm::inverse(view);
//    }
//    static glm::mat4 calculatePerspectiveProjectionMatrix(float fov, float w, float h, float near, float far) {
//        float aspectRatio = w / h;
//        float scaleY = 1.0f / tan(fov / 2.0f);
//        float scaleX = scaleY / aspectRatio;
//        float frustumLen = far - near;
//        glm::mat4 m {scaleX, 0, 0, 0,
//              0, scaleY, 0, 0,
//              0, 0, -((near+far) / frustumLen), -((2.0f*near*far) / frustumLen),
//              0, 0, -1.0f, 0};
//        return glm::transpose(m);
//        return glm::perspective(fov, w/h, near, far);
//    }

    static glm::mat4 matModel(glm::vec3 position) {
        glm::mat4 mat{1};

        mat = glm::translate(mat, position);

        return mat;
    }
    static glm::mat4 matModel(glm::vec3 position, glm::mat3 rotation, glm::vec3 scale) {
        glm::mat4 mat{1};

        mat = glm::translate(mat, position);

        mat = mat * glm::mat4(rotation);

        mat = glm::scale(mat, scale);


        return mat;
    }
    static glm::mat4 matModel(glm::vec3 position, glm::vec3 scale) {
        glm::mat4 mat{1};

        mat = glm::translate(mat, position);

        mat = glm::scale(mat, scale);

        return mat;
    }

    static glm::vec2 ndc(float x, float y, float w, float h) {
        return glm::vec2(x/w, 1.0f - y/h) * 2.0f - 1.0f;
    }
};

#endif //ETHERTIA_MTH_H
