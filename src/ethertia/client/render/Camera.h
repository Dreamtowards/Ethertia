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


class Camera
{
public:
    glm::vec3 position;
    glm::vec3 eulerAngles;  // ORDER: YXZ

    glm::mat4 viewMatrix;

    void update(Window& window) {
        float mx = window.getMouseDX() / 200;
        float my = window.getMouseDY() / 200;

        eulerAngles.x += -my;
        if (window.isKeyDown(GLFW_KEY_Z))
            eulerAngles.z += mx;
        else
            eulerAngles.y += -mx;
        eulerAngles.x = Mth::clamp(eulerAngles.x, -Mth::PI_2, Mth::PI_2);


        glm::mat4 rot = glm::mat4(1);

        rot = glm::translate(rot, position);

        rot = glm::rotate(rot, eulerAngles.y, glm::vec3(0, 1, 0));
        rot = glm::rotate(rot, eulerAngles.x, glm::vec3(1, 0, 0));
        rot = glm::rotate(rot, eulerAngles.z, glm::vec3(0, 0, 1));

        viewMatrix = glm::inverse(rot);
    }

    static glm::vec3 diff(float angle) {
        return glm::rotate(glm::mat4(1), angle, glm::vec3(0, 1, 0)) * glm::vec4(0, 0, -1, 1.0);
    }

};

#endif //ETHERTIA_CAMERA_H
