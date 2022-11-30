//
// Created by Dreamtowards on 2022/8/20.
//

#ifndef ETHERTIA_ENTITYRENDERER_H
#define ETHERTIA_ENTITYRENDERER_H

#include <ethertia/entity/Entity.h>

class EntityRenderer
{
public:

    float debugVar1 = 0;
    float debugVar2 = 0;

//    ShaderProgram shader{Loader::loadAssetsStr("shaders/chunk/chunk.vsh"),
//                         Loader::loadAssetsStr("shaders/chunk/chunk.fsh")};


    void render(Entity* entity, ShaderProgram& shader)
    {
        if (dynamic_cast<EntityMesh*>(entity)) {  // assume it's a 'chunk mesh entity'.
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, MaterialTextures::ATLAS_DIFFUSE->getTextureID());

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, MaterialTextures::ATLAS_NORM->getTextureID());

            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, MaterialTextures::ATLAS_DISP->getTextureID());
        }
        else if (entity->diffuseMap)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, entity->diffuseMap->getTextureID());
        }

        shader.useProgram();

        shader.setVector3f("CameraPos", Ethertia::getCamera()->position);

        shader.setMatrix4f("matModel", Mth::matModel(entity->getPosition(), entity->getRotation(), glm::vec3(1.0f)));
        shader.setMatrix4f("matView", Ethertia::getRenderEngine()->viewMatrix);
        shader.setMatrix4f("matProjection", Ethertia::getRenderEngine()->projectionMatrix);

        shader.setVector3f("cursorPos", Ethertia::getBrushCursor().position);
        shader.setFloat("cursorSize", Ethertia::getBrushCursor().size);

        shader.setFloat("debugVar1", debugVar1);
        shader.setFloat("debugVar2", debugVar2);

        glBindVertexArray(entity->model->vaoId);
        glDrawArrays(GL_TRIANGLES, 0, entity->model->vertexCount);
    }
};

#endif //ETHERTIA_ENTITYRENDERER_H
