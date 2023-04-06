//
// Created by Dreamtowards on 2022/12/19.
//

#ifndef ETHERTIA_HITCURSOR_H
#define ETHERTIA_HITCURSOR_H

#include <ethertia/world/Cell.h>

class Entity;

class HitCursor
{
public:
    bool keepTracking = true;

    bool hit = false;
    glm::vec3 position;
    glm::vec3 normal;
    float length = 0;
    Entity* hitEntity = nullptr;  // always valid if .hit is true.

    bool hitTerrain = false;
    Cell* cell = nullptr;  // may invalid even if .hit/.hitTerrain is true.  but the cell.mtl always is valid.
    float cell_breaking_time = 0;  // sec breaking. >= 0. if not 0 means is breaking.
    glm::vec3 cell_position = {0,0,0};

    float brushSize = 5.0;


    // Called when Nothing hit, detect per frame. from Controls::handleContinuousInput().
    void reset() {
        hit = false;
        position = {};
        hitEntity = nullptr;
        length = 0;

        cell = nullptr;
        cell_breaking_time = 0;
        cell_position = {0,0,0};
        hitTerrain = false;
    }

#define BRUSH_SPHERE 1
#define BRUSH_CUBE   2
};

#endif //ETHERTIA_HITCURSOR_H
