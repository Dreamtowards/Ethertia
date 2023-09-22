//
// Created by Dreamtowards on 2022/5/24.
//

#pragma once

#include <glm/vec3.hpp>
#include <glm/common.hpp>  // glm::min/max

#include <functional>

class AABB
{
public:

    glm::vec3 min;
    glm::vec3 max;

    AABB(const glm::vec3& min, const glm::vec3& max) : min(min), max(max) {}

    AABB() : min(0), max(0) {}

//    static bool intersects(const glm::vec3& min, const glm::vec3& max, glm::vec3 p, int axis) {
//        return p[axis] >= min[axis] && p[axis] < max[axis];
//    }

    bool empty() {
        return min == max;
    }
    glm::vec3 extent() {
        return max - min;
    }

    void wrap(glm::vec3 a, glm::vec3 b) {
        min = glm::min(a, b);
        max = glm::max(a, b);
    }

    void set(glm::vec3 p) {
        min = p;
        max = p;
    }

    static void forEach(glm::vec3 min, glm::vec3 max, const std::function<void(glm::vec3)>& fn) {
        for (float rx = min.x; rx <= max.x; ++rx) {
            for (float ry = min.y; ry <= max.y; ++ry) {
                for (float rz = min.z; rz <= max.z; ++rz) {
                    glm::vec3 p(rx,ry,rz);
                    fn(p);
                }
            }
        }
    }

    static void forCube(float r, const std::function<void(glm::vec3)>& fn) {
        forEach(glm::vec3(std::floor(-r)),
                glm::vec3(std::ceil(r)),
                fn);
    }

    static bool intersectsAxis(const AABB& a, const AABB& b, int axis) {
        return a.min[axis] < b.max[axis] && a.max[axis] > b.min[axis];
    }
};

