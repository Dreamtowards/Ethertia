//
// Created by Dreamtowards on 2022/12/19.
//

#ifndef ETHERTIA_BRUSHCURSOR_H
#define ETHERTIA_BRUSHCURSOR_H

class Entity;

class BrushCursor {
public:
    bool keepTracking = true;
    bool hit = false;
    glm::vec3 position;

    float brushSize = 2.0;

    Entity* hitEntity = nullptr;

    int brushType;
    int brushMaterial;

    void reset() {
        hit = false;
        position = {};
        hitEntity = nullptr;
    }

#define BRUSH_SPHERE 1
#define BRUSH_CUBE   2
};

#endif //ETHERTIA_BRUSHCURSOR_H
