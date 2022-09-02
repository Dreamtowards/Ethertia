//
// Created by Dreamtowards on 2022/8/20.
//

#ifndef ETHERTIA_ENTITYRENDERER_H
#define ETHERTIA_ENTITYRENDERER_H

#include <ethertia/entity/Entity.h>

class EntityRenderer
{
public:

//    ShaderProgram shader{Loader::loadAssetsStr("shaders/chunk/chunk.vsh"),
//                         Loader::loadAssetsStr("shaders/chunk/chunk.fsh")};


    void render(Entity* entity, ShaderProgram& shader)
    {
        if (entity->diffuseMap) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, entity->diffuseMap->getTextureID());
        }

        shader.useProgram();


        shader.setVector3f("CameraPos", Ethertia::getCamera()->position);

        shader.setMatrix4f("matModel", Mth::matModel(entity->getPosition(), entity->getRotation(), glm::vec3(1.0f)));
        shader.setMatrix4f("matView", Ethertia::getRenderEngine()->viewMatrix);
        shader.setMatrix4f("matProjection", Ethertia::getRenderEngine()->projectionMatrix);


        shader.setFloat("CursorSize", Ethertia::getBrushCursor().size);
        shader.setVector3f("CursorPos", Ethertia::getBrushCursor().position);

        glBindVertexArray(entity->model->vaoId);
        glDrawArrays(GL_TRIANGLES, 0, entity->model->vertexCount);
    }
};

#endif //ETHERTIA_ENTITYRENDERER_H
