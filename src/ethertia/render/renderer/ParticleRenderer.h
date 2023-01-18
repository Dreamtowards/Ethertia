//
// Created by Dreamtowards on 2022/9/2.
//

#ifndef ETHERTIA_PARTICLERENDERER_H
#define ETHERTIA_PARTICLERENDERER_H

#include <vector>

#include <ethertia/render/Particle.h>
#include <ethertia/render/shader/ShaderProgram.h>

class ParticleRenderer
{
public:

    std::vector<Particle*> m_Particles;

    ShaderProgram m_ParticleShader =
            Loader::loadShaderProgram("shaders/particle/particle.{}", false);

    Model* M_PLANE_C_STRIP = nullptr;

    ParticleRenderer()
    {

        float pos[] = {
                1.0,-1.0, 0,  // RB
                1.0, 1.0, 0,  // RT
               -1.0,-1.0, 0,  // LB
               -1.0, 1.0, 0,  // LT
        };
        float tex[] = {
                1, 0,
                1, 1,
                0, 0,
                0, 1,
        };
        M_PLANE_C_STRIP = Loader::loadModel(4, {
            {3, pos},
            {2, tex}
        });
    }

    void update(float dt)
    {

    }

    void renderAll()
    {
        m_ParticleShader.useProgram();
        m_ParticleShader.setViewProjection();

        for (Particle* p : m_Particles)
        {
            render(p);
        }
    }

    void render(Particle* particle)
    {


        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, particle->texture->texId);

        m_ParticleShader.useProgram();

        m_ParticleShader.setVector3f("position", particle->position);

        glBindVertexArray(M_PLANE_C_STRIP->vaoId);
//        glDrawArraysInstanced();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, M_PLANE_C_STRIP->vertexCount);
    }

};

#endif //ETHERTIA_PARTICLERENDERER_H
