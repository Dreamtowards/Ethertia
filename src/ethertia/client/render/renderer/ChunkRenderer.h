//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_CHUNKRENDERER_H
#define ETHERTIA_CHUNKRENDERER_H

#include <ethertia/client/render/shader/ShaderProgram.h>
#include <ethertia/client/Loader.h>
#include <ethertia/world/chunk/Chunk.h>
#include <ethertia/init/BlockTextures.h>
#include "Renderer.h"

class ChunkRenderer {

    ShaderProgram shader{Loader::loadAssetsStr("shaders/chunk/chunk.vsh"),
                         Loader::loadAssetsStr("shaders/chunk/chunk.fsh")};

public:

    float fogDensity = 0.02f;
    float fogGradient = 1.5f;

    ChunkRenderer() {

        shader.useProgram();
        shader.setInt("diffuseMap", 0);
    }


    void render(Chunk* chunk) {

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, BlockTextures::ATLAS->atlasTexture->getTextureID());

        shader.useProgram();

        shader.setFloat("fogDensity", fogDensity);
        shader.setFloat("fogGradient", fogGradient);

        Renderer::setShaderCamPos(&shader);
        Renderer::setShaderMVP(&shader, Mth::matModel(chunk->getPosition()));

        glBindVertexArray(chunk->model->vaoId);
        glDrawArrays(GL_TRIANGLES, 0, chunk->model->vertexCount);
    }

};

#endif //ETHERTIA_CHUNKRENDERER_H
