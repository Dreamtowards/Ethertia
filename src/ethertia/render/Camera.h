//
// Created by Dreamtowards on 2022/4/25.
//

#ifndef ETHERTIA_CAMERA_H
#define ETHERTIA_CAMERA_H

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <ethertia/util/Mth.h>
#include <ethertia/util/SmoothValue.h>


class Camera
{
public:
    glm::vec3 position;
    glm::vec3 eulerAngles;  // ORDER: YXZ

    glm::vec3 actual_pos;

    glm::vec3 direction;  // produced by EulerAngles.
    float len;

    /// Smoothness of Camera's Pitch and Yaw.
    /// value is Seconds to the actual Destination Rotation., 0 accidentally means non-smooth.
    float smoothness = 0.0f;

    void updateMovement(float dt, float mDX, float mDY, bool rotZ, float dScroll) {
        float mx = mDX / 200;
        float my = mDY / 200;
        if (rotZ) eulerAngles.z += mx;

        static SmoothValue sX, sY;

        float t = smoothness == 0.0f ? 1.0f : dt / smoothness;

        sY.target += -mx;
        sY.update(t);
        eulerAngles.y = sY.current;

        sX.target += -my;
        sX.target = Mth::clamp(sX.target, -Mth::PI_2, Mth::PI_2);
        sX.update(t);
        eulerAngles.x = sX.current;

        len += dScroll;
        len = Mth::max(len, 0.0f);
//        len = Mth::clamp(len, 0.0f, 1000.0f);

    }

    glm::mat4 computeViewMatrix() {

        direction = Mth::eulerDirection(-eulerAngles.y, -eulerAngles.x);
        // assert(glm::length(direction) == 1.0f);

        glm::vec3 p = position + -direction * len;
        actual_pos = p;

        return Mth::viewMatrix(p, eulerAngles);
    }

};

#endif //ETHERTIA_CAMERA_H
