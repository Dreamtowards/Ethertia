//
// Created by Dreamtowards on 2022/12/19.
//

#pragma once

#include <ethertia/world/Entity.h>

class HitResult
{
public:
    bool enabled = true;
    bool hit = false;

    glm::vec3 position;
    glm::vec3 normal;
    float distance = 0;  // length(origin - hit.position)

    Entity entity;

    bool hitVoxel = false;  // if the hit.entity is a chunk.

    //Cell* cell = nullptr;  // may invalid even if .hit/.hitTerrain is true.  but the cell.mtl always is valid.
    //float cell_breaking_time = 0;  // sec breaking. >= 0. if not 0 means is breaking.


    operator bool() const { return hit; }

};
