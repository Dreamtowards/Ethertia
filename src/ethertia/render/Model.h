//
// Created by Dreamtowards on 2022/4/25.
//

#ifndef ETHERTIA_MODEL_H
#define ETHERTIA_MODEL_H

#include <glad/glad.h>
#include <vector>

// Represents an OpenGL VAO. vertex data has already been transmitted to GPU.
class Model
{
public:
    uint vaoId;
    size_t vertexCount;
    std::vector<uint> vbos;

    Model(uint vao, size_t vcount) : vaoId(vao), vertexCount(vcount) {}

    ~Model() {
        glDeleteBuffers(vbos.size(), &vbos[0]);
        glDeleteVertexArrays(1, &vaoId);
    }

    void drawArrays() {
        glBindVertexArray(vaoId);
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    }
};

#endif //ETHERTIA_MODEL_H
