//
// Created by Dreamtowards on 2022/4/25.
//

#ifndef ETHERTIA_MODEL_H
#define ETHERTIA_MODEL_H

#include <glad/glad.h>
#include <vector>

// A Model / OpenGL VAO. vertex data has already been transmitted to GPU.
class Model
{
public:
    GLuint vaoId;
    size_t vertexCount;
    std::vector<GLuint> vbos;

    Model(GLuint vao, size_t vcount) : vaoId(vao), vertexCount(vcount) {}

    ~Model() {
        glDeleteBuffers(vbos.size(), &vbos[0]);
        glDeleteVertexArrays(1, &vaoId);
    }

    void _glDrawArrays() {
        glBindVertexArray(vaoId);
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    }
};

// More specific. VertexData, VertexArrays, VertexData-AttributeList
//using VertexArrays = Model;


#endif //ETHERTIA_MODEL_H
