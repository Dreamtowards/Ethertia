//
// Created by Dreamtowards on 2024/8/1.
//

#ifndef ETHERTIA_GLX_H
#define ETHERTIA_GLX_H

#include <stdint.h>
#include <glad-gl4.6/include/glad/glad.h>

class Texture {

public:
    Texture(int width, int height, GLuint textureId) : m_Width(width), m_Height(height), m_TextureId(textureId) {}

    int width()  const { return m_Width; }
    int height() const { return m_Height; }

    GLuint textureId() const { return m_TextureId; }

    void* textureIdPtr() const { return (void*)(intptr_t)m_TextureId; }

private:
    int m_Width{};
    int m_Height{};

    GLuint m_TextureId{};
};


class VertexBufferArrays {
public:
    VertexBufferArrays(GLuint vaoId, GLuint vboId, GLuint iboId, size_t vertexCount) :
        m_vaoId(vaoId), m_vboId(vboId), m_iboId(iboId), m_VertexCount(vertexCount) {}

    bool indexed() const { return m_iboId != 0; }

    GLuint m_vaoId = 0;
    GLuint m_vboId = 0;
    GLuint m_iboId = 0;
    size_t m_VertexCount = 0;
};

#endif //ETHERTIA_GLX_H
