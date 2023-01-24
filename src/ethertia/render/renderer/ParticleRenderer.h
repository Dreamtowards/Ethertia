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

    ShaderProgram m_ParticleShader =
            Loader::loadShaderProgram("shaders/particle/particle.{}", false);

    inline static std::vector<Particle*> m_Particles;

    inline static Model* M_PLANE_C_STRIP = nullptr;

    inline static Particle* SUN  = nullptr;
    inline static Particle* MOON = nullptr;

    ParticleRenderer()
    {
        {
            float pos[] = {
                     0.5, -0.5, 0,  // RB
                     0.5,  0.5, 0,  // RT
                    -0.5, -0.5, 0,  // LB
                    -0.5,  0.5, 0,  // LT
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

        {

        }
    }

    void updateAll(float dt)
    {
        std::vector<Particle*> tobeDelete;

        for (Particle* p : m_Particles)
        {
            if (!p->update(dt))
            {
                tobeDelete.push_back(p);
            }
        }

        // Delete particles that out of lifetime.
        for (Particle* p : tobeDelete)
        {
            Collections::erase(m_Particles, p);
            delete p;
        }
    }

    void renderAll()
    {
        m_ParticleShader.useProgram();
        m_ParticleShader.setViewProjection();

        for (Particle* p : m_Particles)
        {
            render(*p);
        }
    }

    void render(const Particle& p)
    {
        render(p.texture, p.position, p.size);
    }

    void render(Texture* tex, glm::vec3 position, float size)
    {

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex->texId);

        m_ParticleShader.useProgram();

        m_ParticleShader.setVector3f("position", position);
        m_ParticleShader.setFloat("size", size);

        glBindVertexArray(M_PLANE_C_STRIP->vaoId);
//        glDrawArraysInstanced();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, M_PLANE_C_STRIP->vertexCount);
    }

};

#endif //ETHERTIA_PARTICLERENDERER_H
