//
// Created by Dreamtowards on 2022/12/19.
//

#ifndef ETHERTIA_BRUSHCURSOR_H
#define ETHERTIA_BRUSHCURSOR_H


class BrushCursor {
public:
    bool keepTracking = true;
    bool hit;
    glm::vec3 position;

    float brushSize = 0.0;

    int brushType;
    int brushMaterial;

#define BRUSH_SPHERE 1
#define BRUSH_CUBE   2
};

#endif //ETHERTIA_BRUSHCURSOR_H
