//
// Created by Dreamtowards on 2022/4/25.
//

#include "ChunkRenderer.h"
#include <ethertia/client/Ethertia.h>


void ChunkRenderer::render(Chunk* chunk) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, BlockTextures::ATLAS->atlasTexture->getTextureID());

    shader.useProgram();

    shader.setVector3f("CameraPos", Ethertia::getCamera()->position);

    shader.setMatrix4f("matModel", glm::translate(glm::mat4(1), chunk->getPosition()));
    shader.setMatrix4f("matView", Ethertia::getRenderEngine()->viewMatrix);
    shader.setMatrix4f("matProjection", Ethertia::getRenderEngine()->projectionMatrix);

//    Log::info("Rendering chunk: vao: "+std::to_string(chunk->model->vaoId)+", vsize: "+std::to_string(chunk->model->vertexCount)+", CamPos: "+glm::to_string(cam->position));

    glBindVertexArray(chunk->model->vaoId);
    glDrawArrays(GL_TRIANGLES, 0, chunk->model->vertexCount);
}