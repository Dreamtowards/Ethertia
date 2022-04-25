//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_CHUNKRENDERER_H
#define ETHERTIA_CHUNKRENDERER_H

#include "ethertia/client/render/shader/ShaderProgram.h"
#include "ethertia/client/Loader.h"

class ChunkRenderer {




public:
    GLuint vao;
    ShaderProgram* shader;

    ChunkRenderer()
    {
        float vts[] = {
                -0.5, -0.5, 0,
                0.5, -0.5, 0,
                0,    0.5, 0
        };
        vao = Loader::loadModel(3, {{vts, 3}});


        shader = new ShaderProgram(Loader::loadAssetsStr("shaders/chunk/chunk.vsh"),
                                   Loader::loadAssetsStr("shaders/chunk/chunk.fsh"));
    }

    void render() const
    {


        shader->useProgram();
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

};

#endif //ETHERTIA_CHUNKRENDERER_H
