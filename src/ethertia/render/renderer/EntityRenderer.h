//
// Created by Dreamtowards on 2022/8/20.
//

#ifndef ETHERTIA_ENTITYRENDERER_H
#define ETHERTIA_ENTITYRENDERER_H

#include <ethertia/entity/Entity.h>
#include <ethertia/entity/EntityMesh.h>
#include <ethertia/init/MaterialTextures.h>

class EntityRenderer
{
public:
    ShaderProgram shaderGeometry{Loader::loadAssetsStr("shaders/chunk/chunk.vsh"),
                         Loader::loadAssetsStr("shaders/chunk/chunk.fsh"),
                         Loader::loadAssetsStr("shaders/chunk/chunk.gsh")};

    ShaderProgram shaderCompose{
        Loader::loadAssetsStr("shaders/chunk/compose.vsh"),
        Loader::loadAssetsStr("shaders/chunk/compose.fsh")
    };

    inline static Model* M_RECT;  // RB,RT,LB,LT. TRIANGLE_STRIP.

    EntityRenderer() {

        shaderGeometry.useProgram();
        shaderGeometry.setInt("diffuseMap", 0);
        shaderGeometry.setInt("normalMap", 1);
        shaderGeometry.setInt("displacementMap", 2);
        shaderGeometry.setInt("roughnessMap", 3);

        shaderGeometry.setFloat("MtlCap", MaterialTextures::TEXTURES.size());


        shaderCompose.useProgram();
        shaderCompose.setInt("gPositionDepth", 0);
        shaderCompose.setInt("gNormal", 1);
        shaderCompose.setInt("gAlbedoRoughness", 2);


        // init RECT. def full viewport.
        float _RECT_POS[] = {1,-1, 1,1, -1,-1, -1,1};
        float _RECT_UV[]  = {1,0,  1,1, 0,0,  0,1};
        M_RECT = Loader::loadModel(4, {
                {2, _RECT_POS},
                {2, _RECT_UV}
        });

    }

    float fogDensity = 0.02f;
    float fogGradient = 1.5f;

    float debugVar1 = 0;
    float debugVar2 = 0;

//    ShaderProgram shader{Loader::loadAssetsStr("shaders/chunk/chunk.vsh"),
//                         Loader::loadAssetsStr("shaders/chunk/chunk.fsh")};


    void renderGeometry(Entity* entity)
    {
        if (dynamic_cast<EntityMesh*>(entity)) {  // assume it's a 'chunk mesh entity'.
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, MaterialTextures::ATLAS_DIFFUSE->texId);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, MaterialTextures::ATLAS_NORM->texId);

            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, MaterialTextures::ATLAS_DISP->texId);

            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, MaterialTextures::ATLAS_ROUGH->texId);
        }
        else if (entity->diffuseMap)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, entity->diffuseMap->texId);
        }

        shaderGeometry.useProgram();

        shaderGeometry.setMatrix4f("matModel", Mth::matModel(entity->getPosition(), entity->getRotation(), glm::vec3(1.0f)));
        shaderGeometry.setMatrix4f("matView", Ethertia::getRenderEngine()->viewMatrix);
        shaderGeometry.setMatrix4f("matProjection", Ethertia::getRenderEngine()->projectionMatrix);

        shaderGeometry.setFloat("debugVar1", debugVar1);
        shaderGeometry.setFloat("debugVar2", debugVar2);


        glBindVertexArray(entity->model->vaoId);
        glDrawArrays(GL_TRIANGLES, 0, entity->model->vertexCount);
    }


    void renderCompose(Texture* gPosition, Texture* gNormal, Texture* gAlbedo) {



        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition->texId);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal->texId);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gAlbedo->texId);

        shaderCompose.useProgram();

        shaderCompose.setVector3f("CameraPos", Ethertia::getCamera()->position);

        shaderCompose.setVector3f("cursorPos", Ethertia::getBrushCursor().position);
        shaderCompose.setFloat("cursorSize", Ethertia::getBrushCursor().size);

        shaderCompose.setFloat("fogGradient", fogGradient);
        shaderCompose.setFloat("fogDensity", fogDensity);

        shaderCompose.setFloat("debugVar1", debugVar1);
        shaderCompose.setFloat("debugVar2", debugVar2);

        glBindVertexArray(EntityRenderer::M_RECT->vaoId);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, M_RECT->vertexCount);
    }
};

#endif //ETHERTIA_ENTITYRENDERER_H
