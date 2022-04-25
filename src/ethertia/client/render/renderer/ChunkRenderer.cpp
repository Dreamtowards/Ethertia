//
// Created by Dreamtowards on 2022/4/25.
//

#include "ChunkRenderer.h"
#include <ethertia/client/Ethertia.h>

void ChunkRenderer::render(Chunk* chunk) const {

    shader->useProgram();

    Camera* cam = Ethertia::getCamera();
    Window* win = Ethertia::getWindow();
//    shader->setVector3f("CameraPos", cam->position);

    glm::mat4 projMat = Mth::calculatePerspectiveProjectionMatrix(90, win->getWidth(), win->getHeight(), 0.1, 1000);

    shader->setMatrix4f("mvpModel", glm::translate(glm::mat4(1), chunk->getPosition()));
    shader->setMatrix4f("mvpView", cam->viewMatrix);
    shader->setMatrix4f("mvpProj", projMat);

//    Log::info("Rendering chunk: vao: "+std::to_string(chunk->model->vaoId)+", vsize: "+std::to_string(chunk->model->vertexCount)+", CamPos: "+glm::to_string(cam->position));

    glBindVertexArray(chunk->model->vaoId);
    glDrawArrays(GL_TRIANGLES, 0, chunk->model->vertexCount);

}
