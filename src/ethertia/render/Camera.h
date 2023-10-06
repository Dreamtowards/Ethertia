//
// Created by Dreamtowards on 2022/4/25.
//

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <ethertia/util/Mth.h>
#include <ethertia/util/Frustum.h>
#include <ethertia/util/SmoothValue.h>
#include <ethertia/util/Log.h>

class Camera
{
public:
    glm::vec3 position;  // always sync with matView. since get position from matView is expensive (mat4 inverse). so if you modified matView directly, don't forget update this position
    
    
    float fov = 90;
    glm::vec3 eulerAngles;  // ORDER: YXZ


    glm::vec3 direction;  // readonly. produced by EulerAngles.

    // readonly.
    glm::mat4 matView{1.0f};
    glm::mat4 matProjection{1.0f};

    Frustum m_Frustum;

    /// Smoothness of Camera's Pitch and Yaw.
    /// value is Seconds to the actual Destination Rotation., 0 accidentally means non-smooth.
    float m_Smoothness = 0.0f;

    void updateMovement(float dt, float mDX, float mDY, bool rotZ)
    {
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
    void update(bool updateMatView = true);

    bool testFrustum(const AABB& aabb) {
        return m_Frustum.intersects(aabb);
    }

};
