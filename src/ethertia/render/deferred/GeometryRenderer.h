//
// Created by Dreamtowards on 2022/8/20.
//

#ifndef ETHERTIA_GEOMETRYRENDERER_H
#define ETHERTIA_GEOMETRYRENDERER_H

#include <random>

#include <ethertia/entity/Entity.h>
#include <ethertia/entity/EntityMesh.h>
#include <ethertia/init/MaterialTextures.h>
#include <ethertia/render/particle/ParticleRenderer.h>
#include <ethertia/render/RenderCommand.h>

class GeometryRenderer
{
public:
    DECL_SHADER(SHADER, "shaders/chunk/geometry.{}");

    inline static Framebuffer* fboGbuffer = nullptr;   // Geometry Buffer FBO, enable MRT (Mutliple Render Targets)

    inline static float u_MaterialTextureScale = 3.5;

    static void init() {

        SHADER->useProgram();
        SHADER->setInt("diffuseMap", 0);
        SHADER->setInt("normalMap", 1);
        SHADER->setInt("dramMap", 2);

        SHADER->setFloat("MtlCap", Material::REGISTRY.size());

        SHADER->m_Uniforms["MtlTexScale"].bind(&u_MaterialTextureScale);


        float qual = 0.7;
        fboGbuffer = Framebuffer::GenFramebuffer((int)(1280 * qual), (int)(720 * qual), [](Framebuffer* fbo)
        {
            fbo->attachColorTexture(0, GL_RGBA32F, GL_RGBA, GL_FLOAT);      // Positions, Depth, f16 *3
            fbo->attachColorTexture(1, GL_RGB32F, GL_RGB, GL_FLOAT);        // Normals,          f16 *3
            fbo->attachColorTexture(2, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE); // Albedo,           u8 *3
            fbo->setupMRT({0, 1, 2});

            fbo->attachDepthStencilRenderbuffer();
        });
    }



    static void renderGeometryChunk(Model* vao, glm::vec3 pos, glm::mat3 rot, Texture* diff, float vertexWaving = 0)
    {
        if (diff)
        {
            diff->BindTexture(0);
        }
        else
        {
            MaterialTextures::ATLAS_DIFFUSE->BindTexture(0);
            MaterialTextures::ATLAS_NORM->BindTexture(1);
            MaterialTextures::ATLAS_DRAM->BindTexture(2);

//                // Experimental
//                glActiveTexture(GL_TEXTURE5);
//                glBindTexture(GL_TEXTURE_2D, g_PanoramaTex->texId);
        }


        SHADER->useProgram();

        // todo: Duplicated VP Set
        SHADER->setViewProjection();
        SHADER->setMatrix4f("matModel", Mth::matModel(pos, rot, glm::vec3(1.0f)));
//
//        SHADER->setFloat("debugVar1", debugVar1);
//        SHADER->setFloat("debugVar2", debugVar2);

        SHADER->setFloat("Time", Ethertia::getPreciseTime());

        SHADER->setFloat("WaveFactor", vertexWaving);

        vao->_glDrawArrays();
//        RenderCommand::DrawArrays(vao);
    }




};

#endif //ETHERTIA_GEOMETRYRENDERER_H
