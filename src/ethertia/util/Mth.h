//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_MTH_H
#define ETHERTIA_MTH_H

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <string>

#include <bullet3/src/LinearMath/btVector3.h>
#include <bullet3/src/LinearMath/btQuaternion.h>
#include <bullet3/src/LinearMath/btTransform.h>

#include <ostream>

namespace glm {
    std::ostream& operator<<(std::ostream& s, const glm::vec3& v);
}

class Mth
{
public:
    static constexpr float PI = 3.14159265359f;
    static constexpr float PI_2 = 1.57079632679f;
    static constexpr float _2PI = 2*PI;

    static constexpr float NaN = std::numeric_limits<float>::quiet_NaN();
    static constexpr float Inf = std::numeric_limits<float>::infinity();

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
        // no floor(x)+1, it causes ceil(1) = 2
        return std::ceil(v);
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

//    static glm::vec3 mod(glm::vec3 v, float f) {
//        return glm::vec3{
//            Mth::mod(v.x, f),
//            Mth::mod(v.y, f),
//            Mth::mod(v.z, f)
//        };
//    }

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

    static float hash(int i) {
        i = (i << 13) ^ i;
        return (((i * i * 15731 + 789221) * i + 1376312589) & 0xffffffff) / (float)0xffffffff;
    }

    static float floor_dn(float f, int n) {
        float p = std::pow(10, n);
        return Mth::floor(f * p) / p;
    }

    static glm::vec3 angleh(float angle) {
        return glm::rotate(glm::mat4(1), angle, glm::vec3(0, 1, 0)) * glm::vec4(0, 0, -1, 1.0);
    }
    static glm::vec3 anglez(float angle) {
        return glm::rotate(glm::mat4(1), angle, glm::vec3(0, 0, 1)) * glm::vec4(1, 0, 0, 1.0);
    }
    static glm::mat4 rot(glm::vec3 axis, float angle) {
        return glm::rotate(glm::mat4(1), angle, axis);
    }
    static glm::vec3 rot_dir(float angle, glm::vec3 axis, glm::vec3 p) {
        return glm::normalize(glm::vec3(glm::rotate(glm::mat4(1), angle, axis) * glm::vec4(p, 0.0f)));
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
    static inline glm::vec3 vec3(const std::string* argv) {
        return glm::vec3(std::stof(argv[0]), std::stof(argv[1]), std::stof(argv[2]));
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







//
// 快速傅立叶变换 Fast Fourier Transform
// By rappizit@yahoo.com.cn
// 2007-07-20
// 版本 2.0
// 改进了《算法导论》的算法，旋转因子取 ωn-kj  (ωnkj 的共轭复数)
// 且只计算 n / 2 次，而未改进前需要计算 (n * lg n) / 2 次。
//

//            float* real = new float[samples];
//            float* imag = new float[samples];
//            for (int i = 0; i < samples; ++i) {
//                int16_t val_raw = buf[i];
//                float f = (float)val_raw / 32768.0f;
//                real[i] = f;
//                imag[i] = f;
//            }
//            Mth::FFT(real, imag, samples);

    inline static void swap(float& a, float& b) {
        float t = a;
        a = b;
        b = t;
    }

    // Bit-reversal Permutation
    static void bitrp(float xreal[], float ximag[], int n)
    {
        int log2n = 0;
        for (int i = 1; i < n; i *= 2) {
            log2n ++;
        }
        for (int i = 0; i < n; i ++)
        {
            int a = i;
            int b = 0;
            for (int j = 0; j < log2n; j ++)
            {
                b = (b << 1) | (a & 1);    // b = b * 2 + a % 2;
                a >>= 1;        // a = a / 2;
            }
            if (b > i)
            {
                assert(b > 0 && b < n);
                swap (xreal [i], xreal [b]);
                swap (ximag [i], ximag [b]);
            }
        }
    }


    static void FFT(float xreal[], float ximag[], int n)
    {
        assert((n & (n - 1)) == 0 && "invalid input size");

        const int N = 1024;
        assert(n <= N);
        float wreal [N / 2], wimag [N / 2], treal, timag, ureal, uimag, arg;
        int m, k, j, t, index1, index2;

        bitrp (xreal, ximag, n);

//        return;

        // 计算 1 的前 n / 2 个 n 次方根的共轭复数 W'j = wreal [j] + i * wimag [j] , j = 0, 1, ... , n / 2 - 1
        arg = - 2 * PI / n;
        treal = cos (arg);
        timag = sin (arg);
        wreal [0] = 1.0;
        wimag [0] = 0.0;
        for (j = 1; j < n / 2; j ++)
        {
            wreal [j] = wreal [j - 1] * treal - wimag [j - 1] * timag;
            wimag [j] = wreal [j - 1] * timag + wimag [j - 1] * treal;
        }

        for (m = 2; m <= n; m *= 2)
        {
            for (k = 0; k < n; k += m)
            {
                for (j = 0; j < m / 2; j ++)
                {
                    index1 = k + j;
                    index2 = index1 + m / 2;
                    t = n * j / m;    // 旋转因子 w 的实部在 wreal [] 中的下标为 t
                    treal = wreal [t] * xreal [index2] - wimag [t] * ximag [index2];
                    timag = wreal [t] * ximag [index2] + wimag [t] * xreal [index2];
                    ureal = xreal [index1];
                    uimag = ximag [index1];
                    xreal [index1] = ureal + treal;
                    ximag [index1] = uimag + timag;
                    xreal [index2] = ureal - treal;
                    ximag [index2] = uimag - timag;
                }
            }
        }
    }


};

#endif //ETHERTIA_MTH_H
