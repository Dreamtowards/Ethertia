//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_CHUNKRENDERER_H
#define ETHERTIA_CHUNKRENDERER_H

#include <ethertia/client/render/shader/ShaderProgram.h>
#include <ethertia/client/render/Camera.h>
#include <ethertia/client/Loader.h>
#include <ethertia/world/Chunk.h>
#include <ethertia/init/BlockTextures.h>

class ChunkRenderer {

public:
    ShaderProgram shader{Loader::loadAssetsStr("shaders/chunk/chunk.vsh"),
                         Loader::loadAssetsStr("shaders/chunk/chunk.fsh")};


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

        shader.setVector3f("CameraPos", Ethertia::getCamera()->position);

        shader.setMatrix4f("matModel", Mth::matModel(chunk->position));
        shader.setMatrix4f("matView", Ethertia::getRenderEngine()->viewMatrix);
        shader.setMatrix4f("matProjection", Ethertia::getRenderEngine()->projectionMatrix);

        glBindVertexArray(chunk->model->vaoId);
        glDrawArrays(GL_TRIANGLES, 0, chunk->model->vertexCount);
    }

};

#endif //ETHERTIA_CHUNKRENDERER_H
