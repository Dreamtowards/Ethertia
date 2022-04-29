//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_MTH_H
#define ETHERTIA_MTH_H

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

class Mth
{
public:
    static constexpr float PI = 3.1416;
    static constexpr float PI_2 = 1.5708;

    static inline int floor(float v, int u) {
        int i = (int)floor(v / (float)u);
        return i*u;
    }

    static inline int floor(float v) {
        int i = (int)v;
        return (float)i > v ? i-1 : i;
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
    static inline T fade(T t) {
        return t*t*t*(t*(t*6-15)+10);
    }
    template<typename T>
    static inline T lerp(T t, T a, T b) {
        return a + t * (b - a);
    }
    template<typename T>
    static inline T clamp(T f, T a, T b) {
        return min(max(f, a), b);
    }

    static glm::mat4 calculateViewMatrix(glm::vec3 position, glm::mat3 rotation) {
        glm::mat4 view = glm::mat4(1);// = glm::mat4(rotation);

        view = glm::translate(view, position);

        return glm::inverse(view);
    }
    static glm::mat4 calculatePerspectiveProjectionMatrix(float fov, float w, float h, float near, float far) {
//        float aspectRatio = w / h;
//        float scaleY = 1.0f / tan(fov / 2.0f);
//        float scaleX = scaleY / aspectRatio;
//        float frustumLen = far - near;
//        glm::mat4 m {scaleX, 0, 0, 0,
//              0, scaleY, 0, 0,
//              0, 0, -((near+far) / frustumLen), -((2.0f*near*far) / frustumLen),
//              0, 0, -1.0f, 0};
//        return glm::transpose(m);
        return glm::perspective(fov, w/h, near, far);
    }

    static glm::vec2 ndc(float x, float y, float w, float h) {
        return glm::vec2(x/w, 1.0f - y/h) * 2.0f - 1.0f;
    }
};

#endif //ETHERTIA_MTH_H
