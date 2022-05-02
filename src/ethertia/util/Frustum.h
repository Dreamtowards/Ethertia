//
// Created by Dreamtowards on 2022/5/3.
//

#ifndef ETHERTIA_FRUSTUM_H
#define ETHERTIA_FRUSTUM_H

#include <glm/vec4.hpp>
#include <glm/gtc/matrix_transform.hpp>

// http://www.cs.otago.ac.nz/postgrads/alexis/planeExtraction.pdf
// https://www.flipcode.com/archives/Frustum_Culling.shtml
// https://iquilezles.org/www/articles/frustumcorrect/frustumcorrect.htm
class Frustum
{
    glm::vec4 planes[6];

    void set(glm::mat4 m) {

    }

    bool contains(glm::vec3 p) {

    }

    bool intersects(glm::vec3 min, glm::vec3 max) {

    }
};

#endif //ETHERTIA_FRUSTUM_H
