//
// Created by Dreamtowards on 2022/4/25.
//

#ifndef ETHERTIA_CAMERA_H
#define ETHERTIA_CAMERA_H

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <ethertia/client/Window.h>
#include <ethertia/util/Mth.h>
#include <ethertia/util/SmoothValue.h>


class Camera
{
public:
    glm::vec3 position;
    glm::vec3 eulerAngles;  // ORDER: YXZ

    glm::vec3 direction;  // produced by EulerAngles.
    float len;


    void update(Window& window, float dt, glm::mat4& out_viewMatrix) {
        float mx = window.getMouseDX() / 200;
        float my = window.getMouseDY() / 200;

        eulerAngles.x += -my;
        if (window.isKeyDown(GLFW_KEY_Z)) eulerAngles.z += mx;
        else eulerAngles.y += -mx;

        static SmoothValue smX, smY;

        float stp = dt / 0.5f;

        smY.target += -mx;
        eulerAngles.y = smY.current;
        smY.update(stp);

        smX.target += -my;
        eulerAngles.x = smX.current;
        smX.update(stp);

        eulerAngles.x = Mth::clamp(eulerAngles.x, -Mth::PI_2, Mth::PI_2);


        direction = Mth::eulerDirection(-eulerAngles.y, -eulerAngles.x);

        len += window.getDScroll();
        len = Mth::clamp(len, 0.0f, 100.0f);

        glm::vec3 pos = position + -direction * len;

        out_viewMatrix = Mth::viewMatrix(pos, eulerAngles);
    }

};

#endif //ETHERTIA_CAMERA_H
