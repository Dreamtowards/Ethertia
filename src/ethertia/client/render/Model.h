//
// Created by Dreamtowards on 2022/4/25.
//

#ifndef ETHERTIA_MODEL_H
#define ETHERTIA_MODEL_H

#include <glad/glad.h>
#include <vector>

class Model
{
public:
    uint vaoId;
    uint vertexCount;

    std::vector<uint> vbos;

    Model(uint vao, uint vcount) : vaoId(vao), vertexCount(vcount) {}

    ~Model() {
        glDeleteBuffers(vbos.size(), &vbos[0]);

        glDeleteVertexArrays(1, &vaoId);
    }
};

#endif //ETHERTIA_MODEL_H
