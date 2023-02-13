//
// Created by Dreamtowards on 2022/8/20.
//

#ifndef ETHERTIA_ENTITYRENDERER_H
#define ETHERTIA_ENTITYRENDERER_H

#include <random>

#include <ethertia/entity/Entity.h>
#include <ethertia/entity/EntityMesh.h>
#include <ethertia/init/MaterialTextures.h>
#include <ethertia/render/renderer/ParticleRenderer.h>

class EntityRenderer
{
public:

    ShaderProgram shaderGeometry = Loader::loadShaderProgram("shaders/chunk/geometry.{}", true);
//    ShaderProgram shaderCompose = Loader::loadShaderProgram("shaders/chunk/compose.{}");
//
//    inline static Model* M_RECT;  // RB,RT,LB,LT. TRIANGLE_STRIP.

//    inline static Texture* g_PanoramaTex = nullptr;

    inline static float fogDensity = 0.02f;
    inline static float fogGradient = 1.5f;

    inline static float debugVar0 = 0;
    inline static float debugVar1 = 1;
    inline static float debugVar2 = 0;

//    inline static glm::vec3 SunPos;

    EntityRenderer() {

        shaderGeometry.useProgram();
        shaderGeometry.setInt("diffuseMap", 0);
        shaderGeometry.setInt("normalMap", 1);
        shaderGeometry.setInt("dramMap", 2);

        shaderGeometry.setFloat("MtlCap", Material::REGISTRY.size());


//        shaderCompose.useProgram();
//        shaderCompose.setInt("gPositionDepth", 0);
//        shaderCompose.setInt("gNormal", 1);
//        shaderCompose.setInt("gAlbedoRoughness", 2);
//
//        shaderCompose.setInt("panoramaMap", 5);


//        // init RECT. def full viewport.
//        float _RECT_POS[] = {1,-1, 1,1, -1,-1, -1,1};
//        float _RECT_UV[]  = {1,0,  1,1, 0,0,  0,1};
//        M_RECT = Loader::loadModel(4, {
//                {2, _RECT_POS},
//                {2, _RECT_UV}
//        });
//
//        g_PanoramaTex = Loader::loadTexture(Loader::loadPNG(Loader::loadAssets("misc/skybox/hdri5.png")));

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


        shaderGeometry.useProgram();

        shaderGeometry.setViewProjection();
        shaderGeometry.setMatrix4f("matModel", Mth::matModel(pos, rot, glm::vec3(1.0f)));

        shaderGeometry.setFloat("debugVar1", debugVar1);
        shaderGeometry.setFloat("debugVar2", debugVar2);

        shaderGeometry.setFloat("Time", Ethertia::getPreciseTime());

        shaderGeometry.setFloat("WaveFactor", vertexWaving);


        glBindVertexArray(model->vaoId);
        glDrawArrays(GL_TRIANGLES, 0, model->vertexCount);
    }




};

#endif //ETHERTIA_ENTITYRENDERER_H
