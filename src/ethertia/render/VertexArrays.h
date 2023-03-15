//
// Created by Dreamtowards on 2023/3/13.
//

#ifndef ETHERTIA_VERTEXARRAYS_H
#define ETHERTIA_VERTEXARRAYS_H

class VertexArrays
{
    VertexArrays() = default;
public:
    int vaoId = 0;
    int vboId[8] = {};
    size_t vertexCount = 0;

    static VertexArrays* GenVertexArrays();
    ~VertexArrays();

    void BindVertexArrays();
};

#endif //ETHERTIA_VERTEXARRAYS_H
