//
// Created by Dreamtowards on 2022/4/25.
//

#ifndef ETHERTIA_MODEL_H
#define ETHERTIA_MODEL_H

class Model
{
public:
    uint vaoId;
    uint vertexCount;

    Model(uint vao, uint vcount) : vaoId(vao), vertexCount(vcount) {}
};

#endif //ETHERTIA_MODEL_H
