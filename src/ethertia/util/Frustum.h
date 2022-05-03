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
    // planes
    glm::vec4 ps[6];

public:

    // RH -> LH.
    void set(glm::mat4 m) {
        ps[0] = glm::vec4(m[0][3] + m[0][0], m[1][3] + m[1][0], m[2][3] + m[2][0], m[3][3] + m[3][0]);
        ps[1] = glm::vec4(m[0][3] - m[0][0], m[1][3] - m[1][0], m[2][3] - m[2][0], m[3][3] - m[3][0]);
        ps[2] = glm::vec4(m[0][3] + m[0][1], m[1][3] + m[1][1], m[2][3] + m[2][1], m[3][3] + m[3][1]);
        ps[3] = glm::vec4(m[0][3] - m[0][1], m[1][3] - m[1][1], m[2][3] - m[2][1], m[3][3] - m[3][1]);
        ps[4] = glm::vec4(m[0][3] + m[0][2], m[1][3] + m[1][2], m[2][3] + m[2][2], m[3][3] + m[3][2]);
        ps[5] = glm::vec4(m[0][3] - m[0][2], m[1][3] - m[1][2], m[2][3] - m[2][2], m[3][3] - m[3][2]);
    }

    bool contains(glm::vec3 p) {
        float x=p.x, y=p.y, z=p.z;
        return ps[0].x * x + ps[0].y * y + ps[0].z * z + ps[0].w >= 0 &&
               ps[1].x * x + ps[1].y * y + ps[1].z * z + ps[1].w >= 0 &&
               ps[2].x * x + ps[2].y * y + ps[2].z * z + ps[2].w >= 0 &&
               ps[3].x * x + ps[3].y * y + ps[3].z * z + ps[3].w >= 0 &&
               ps[4].x * x + ps[4].y * y + ps[4].z * z + ps[4].w >= 0 &&
               ps[5].x * x + ps[5].y * y + ps[5].z * z + ps[5].w >= 0;
    }

    bool intersects(glm::vec3 min, glm::vec3 max) {
        float minX=min.x, minY=min.y, minZ=min.z, maxX=max.x, maxY=max.y, maxZ=max.z;
        return ps[0].x * (ps[0].x < 0 ? minX : maxX) + ps[0].y * (ps[0].y < 0 ? minY : maxY) + ps[0].z * (ps[0].z < 0 ? minZ : maxZ) >= -ps[0].w &&
               ps[1].x * (ps[1].x < 0 ? minX : maxX) + ps[1].y * (ps[1].y < 0 ? minY : maxY) + ps[1].z * (ps[1].z < 0 ? minZ : maxZ) >= -ps[1].w &&
               ps[2].x * (ps[2].x < 0 ? minX : maxX) + ps[2].y * (ps[2].y < 0 ? minY : maxY) + ps[2].z * (ps[2].z < 0 ? minZ : maxZ) >= -ps[2].w &&
               ps[3].x * (ps[3].x < 0 ? minX : maxX) + ps[3].y * (ps[3].y < 0 ? minY : maxY) + ps[3].z * (ps[3].z < 0 ? minZ : maxZ) >= -ps[3].w &&
               ps[4].x * (ps[4].x < 0 ? minX : maxX) + ps[4].y * (ps[4].y < 0 ? minY : maxY) + ps[4].z * (ps[4].z < 0 ? minZ : maxZ) >= -ps[4].w &&
               ps[5].x * (ps[5].x < 0 ? minX : maxX) + ps[5].y * (ps[5].y < 0 ? minY : maxY) + ps[5].z * (ps[5].z < 0 ? minZ : maxZ) >= -ps[5].w;
    }
};

#endif //ETHERTIA_FRUSTUM_H
