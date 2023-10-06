//
// Created by Dreamtowards on 2022/12/19.
//

#pragma once


class Entity;

class HitResult
{
public:
    bool enabled = true;
    bool hit = false;

    glm::vec3 position;
    glm::vec3 normal;
    float distance = 0;  // length(origin - hit.position)

    Entity entity;  // always valid if .hit is true.

    bool hitVoxel = false;
    //Cell* cell = nullptr;  // may invalid even if .hit/.hitTerrain is true.  but the cell.mtl always is valid.
    //float cell_breaking_time = 0;  // sec breaking. >= 0. if not 0 means is breaking.


    operator bool() const { return hit; }

};
