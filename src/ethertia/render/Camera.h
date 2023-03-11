//
// Created by Dreamtowards on 2022/4/25.
//

#ifndef ETHERTIA_CAMERA_H
#define ETHERTIA_CAMERA_H

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <ethertia/util/Mth.h>
#include <ethertia/util/Frustum.h>
#include <ethertia/util/SmoothValue.h>
#include <ethertia/util/Log.h>

class Camera
{
public:
    glm::vec3 position;
    float fov = 90;
    glm::vec3 eulerAngles;  // ORDER: YXZ

    glm::vec3 actual_pos;

    glm::vec3 direction;  // readonly. produced by EulerAngles.
    float len;

    // readonly.
    glm::mat4 matView{1.0f};
    glm::mat4 matProjection{1.0f};

    Frustum m_Frustum;

    /// Smoothness of Camera's Pitch and Yaw.
    /// value is Seconds to the actual Destination Rotation., 0 accidentally means non-smooth.
    float m_Smoothness = 0.0f;

    void updateMovement(float dt, float mDX, float mDY, bool rotZ, float dScroll) {
        float mx = mDX / 200;
        float my = mDY / 200;
        float mz = 0;
        if (rotZ) mz += mx;

        static SmoothValue sX, sY, sZ;

        float t = m_Smoothness == 0.0f ? 1.0f : dt / m_Smoothness;

        sY.target += -mx;
        sY.update(t);
        eulerAngles.y = sY.current;

        sX.target += -my;
        sX.target = Mth::clamp(sX.target, -Mth::PI_2, Mth::PI_2);
        sX.update(t);
        eulerAngles.x = sX.current;

        sZ.target += mz / 20.0f;
        sZ.update(t);
        eulerAngles.z = sZ.current;

    }

    // update ViewMatrix, ProjectionMatrix, and Frustum, etc.
    void update();

};

#endif //ETHERTIA_CAMERA_H
