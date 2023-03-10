//
// Created by Dreamtowards on 2022/8/20.
//

#ifndef ETHERTIA_ENTITYRENDERER_H
#define ETHERTIA_ENTITYRENDERER_H

#include <random>

#include <ethertia/entity/Entity.h>
#include <ethertia/entity/EntityMesh.h>
#include <ethertia/init/MaterialTextures.h>
#include <ethertia/render/particle/ParticleRenderer.h>

class EntityRenderer
{
public:
    DECL_SHADER(SHADER, "shaders/chunk/geometry.{}");

    inline static float debugVar0 = 0;
    inline static float debugVar1 = 1;
    inline static float debugVar2 = 0;


    EntityRenderer() {

        SHADER->useProgram();
        SHADER->setInt("diffuseMap", 0);
        SHADER->setInt("normalMap", 1);
        SHADER->setInt("dramMap", 2);

        SHADER->setFloat("MtlCap", Material::REGISTRY.size());

    }



    void renderGeometryChunk(Model* model, glm::vec3 pos, glm::mat3 rot, Texture* diff, float vertexWaving = 0)
    {
        if (diff)
        {
            diff->bindTexture2D(0);
        }
        else
        {
            MaterialTextures::ATLAS_DIFFUSE->bindTexture2D(0);
            MaterialTextures::ATLAS_NORM->bindTexture2D(1);
            MaterialTextures::ATLAS_DRAM->bindTexture2D(2);

//                // Experimental
//                glActiveTexture(GL_TEXTURE5);
//                glBindTexture(GL_TEXTURE_2D, g_PanoramaTex->texId);
        }


        SHADER->useProgram();

        // todo: Duplicated VP Set
        SHADER->setViewProjection();
        SHADER->setMatrix4f("matModel", Mth::matModel(pos, rot, glm::vec3(1.0f)));

        SHADER->setFloat("debugVar1", debugVar1);
        SHADER->setFloat("debugVar2", debugVar2);

        SHADER->setFloat("Time", Ethertia::getPreciseTime());

        SHADER->setFloat("WaveFactor", vertexWaving);


        glBindVertexArray(model->vaoId);
        glDrawArrays(GL_TRIANGLES, 0, model->vertexCount);
    }




};

#endif //ETHERTIA_ENTITYRENDERER_H
