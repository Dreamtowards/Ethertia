//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_CHUNKRENDERER_H
#define ETHERTIA_CHUNKRENDERER_H

#include <ethertia/render/shader/ShaderProgram.h>
#include <ethertia/render/Camera.h>
#include <ethertia/util/Loader.h>
#include <ethertia/world/Chunk.h>

class ChunkRenderer {

public:
    ShaderProgram shader{Loader::loadAssetsStr("shaders/chunk/chunk.vsh"),
                         Loader::loadAssetsStr("shaders/chunk/chunk.fsh"),
                         Loader::loadAssetsStr("shaders/chunk/chunk.gsh")};


    float fogDensity = 0.02f;
    float fogGradient = 1.5f;

//    inline static const char** _MaterialAtlasRegions = ShaderProgram::_GenArrayNames("MaterialAtlasRegions[%i]", 128);

    ChunkRenderer() {

        shader.useProgram();
        shader.setInt("diffuseMap", 0);
        shader.setInt("normalMap", 1);
        shader.setInt("displacementMap", 2);

//        // init MaterialAtlasRegions table.  from id 1.
//        for (int i = 1; i < Materials::_SIZE; ++i) {
//            TextureAtlas::Region* r = MaterialTextures::of(i);
//            shader.setVector4f(_MaterialAtlasRegions[i], glm::vec4(r->pos.x, r->pos.y, r->size.x, r->size.y));
//
//            // Log::info("init _MaterialAtlasRegions {} :: {}, {}", i, glm::to_string(r->pos), glm::to_string(r->size));
//        }
    }


//    void render(Chunk* chunk) {
//
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, MaterialTextures::ATLAS->atlasTexture->getTextureID());
//
//        shader.useProgram();
//
//        shader.setFloat("fogDensity", fogDensity);
//        shader.setFloat("fogGradient", fogGradient);
//
//        shader.setVector3f("CameraPos", Ethertia::getCamera()->position);
//
//        shader.setMatrix4f("matModel", Mth::matModel(chunk->position));
//        shader.setMatrix4f("matView", Ethertia::getRenderEngine()->viewMatrix);
//        shader.setMatrix4f("matProjection", Ethertia::getRenderEngine()->projectionMatrix);
//
//        glBindVertexArray(chunk->proxy->model->vaoId);
//        glDrawArrays(GL_TRIANGLES, 0, chunk->proxy->model->vertexCount);
//    }

};

#endif //ETHERTIA_CHUNKRENDERER_H
