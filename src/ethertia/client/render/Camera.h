//
// Created by Dreamtowards on 2022/4/25.
//

#ifndef ETHERTIA_CAMERA_H
#define ETHERTIA_CAMERA_H

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <ethertia/client/Window.h>

#include <cmath>

class Camera
{
public:
    glm::vec3 position;
    glm::vec3 eulerAngles;  // ORDER: YXZ

    glm::mat4 viewMatrix;

    static void update(Camera& cam, Window& window) {
        cam.eulerAngles.x += -window.getMouseDY() / 200;
        if (window.isAltKeyDown())
            cam.eulerAngles.z += window.getMouseDX() / 200;
        else
            cam.eulerAngles.y += -window.getMouseDX() / 200;


        glm::mat4 rot = glm::mat4(1);

        rot = glm::translate(rot, cam.position);

        rot = glm::rotate(rot, cam.eulerAngles.y, glm::vec3(0, 1, 0));
        rot = glm::rotate(rot, cam.eulerAngles.x, glm::vec3(1, 0, 0));
        rot = glm::rotate(rot, cam.eulerAngles.z, glm::vec3(0, 0, 1));

        cam.viewMatrix = glm::inverse(rot);
    }

    static glm::vec3 diff(float angle) {
        return glm::rotate(glm::mat4(1), angle, glm::vec3(0, 1, 0)) * glm::vec4(0, 0, -1, 1.0);
    }

};

#endif //ETHERTIA_CAMERA_H
